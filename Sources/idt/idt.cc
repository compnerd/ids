// Copyright (c) 2021 Saleem Abdulrasool.  All Rights Reserved.
// SPDX-License-Identifier: BSD-3-Clause

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace idt {
llvm::cl::OptionCategory category{"interface definition scanner options"};
}

namespace {
// TODO(compnerd) make this configurable via a configuration file or commandline
const std::set<std::string> kIgnoredBuiltins{
  "_BitScanForward",
  "_BitScanForward64",
  "_BitScanReverse",
  "_BitScanReverse64",
  "__builtin_strlen",
};

llvm::cl::opt<std::string>
export_macro("export-macro",
             llvm::cl::desc("The macro to decorate interfaces with"),
             llvm::cl::value_desc("define"), llvm::cl::Required,
             llvm::cl::cat(idt::category));

llvm::cl::opt<std::string>
include_header("include-header",
               llvm::cl::desc("Header required for export macro"),
               llvm::cl::value_desc("header"),
               llvm::cl::cat(idt::category));

llvm::cl::opt<bool>
apply_fixits("apply-fixits", llvm::cl::init(false),
             llvm::cl::desc("Apply suggested changes to decorate interfaces"),
             llvm::cl::cat(idt::category));

llvm::cl::opt<bool>
inplace("inplace", llvm::cl::init(false),
        llvm::cl::desc("Apply suggested changes in-place"),
        llvm::cl::cat(idt::category));

llvm::cl::list<std::string>
ignored_symbols("ignore",
                llvm::cl::desc("Ignore one or more functions"),
                llvm::cl::value_desc("function-name[,function-name...]"),
                llvm::cl::CommaSeparated,
                llvm::cl::cat(idt::category));

template <typename Key, typename Compare, typename Allocator>
bool contains(const std::set<Key, Compare, Allocator>& set, const Key& key) {
  return set.find(key) != set.end();
}

const std::set<std::string> &get_ignored_symbols() {
  static auto kIgnoredFunctions = [&]() -> std::set<std::string> {
      return { ignored_symbols.begin(), ignored_symbols.end() };
    }();

  return kIgnoredFunctions;
}

}

namespace idt {
struct PPCallbacks : clang::PPCallbacks {
  // Describes the source location of an #include statement and the name of the
  // file being included.
  using IncludeLocation = std::tuple<std::string, clang::SourceLocation>;

  // Maps the name of a source file to list of include statements its contains
  // in the order they are discovered int he file.
  using FileIncludes =
      std::unordered_map<std::string, std::vector<IncludeLocation>>;

  PPCallbacks(clang::SourceManager &source_manager, FileIncludes &file_includes)
      : source_manager_(source_manager), file_includes_(file_includes) {}

  void
  InclusionDirective(clang::SourceLocation HashLoc,
                     const clang::Token &IncludeTok, clang::StringRef FileName,
                     bool IsAngled, clang::CharSourceRange FilenameRange,
                     clang::OptionalFileEntryRef File,
                     clang::StringRef SearchPath, clang::StringRef RelativePath,
                     const clang::Module *SuggestedModule, bool ModuleImported,
                     clang::SrcMgr::CharacteristicKind FileType) override {
    // Only track #include statements not #import statements.
    if (ModuleImported)
      return;

    // Track the name and location of each include in the order discovered.
    clang::SourceLocation SLoc = source_manager_.getSpellingLoc(HashLoc);

    // Get the name of the file that contains the #include statement. This
    // string is distinct from the FileName function parameter, which is the
    // name of the include target (e.g. #include <FileName>).
    std::string containingFileName = source_manager_.getFilename(SLoc).str();

    // Only add the include to the list if it isn't already present.
    auto &includes = file_includes_[containingFileName];
    if (std::none_of(includes.begin(), includes.end(),
                     [&FileName](const IncludeLocation &include) {
                       return std::get<0>(include) == FileName.str();
                     }))
      includes.emplace_back(FileName.str(), SLoc);
  }

private:
  clang::SourceManager &source_manager_;
  FileIncludes &file_includes_;
};

// Track a set of clang::Decl declarations by unique ID.
class DeclSet {
  std::set<std::uintptr_t> decls_;

  // Use the raw address of the canonical declaration object as a unique
  // identifier.
  template <typename Decl_>
  std::uintptr_t decl_id(const Decl_ *D) const {
    return reinterpret_cast<std::uintptr_t>(D->getCanonicalDecl());
  }

public:
  template <typename Decl_>
  inline void insert(const Decl_ *D) {
    decls_.insert(decl_id(D));
  }

  template <typename Decl_>
  inline bool contains(const Decl_ *D) const {
    return decls_.find(decl_id(D)) != decls_.end();
  }
};

class visitor : public clang::RecursiveASTVisitor<visitor> {
  clang::ASTContext &context_;
  clang::SourceManager &source_manager_;
  std::optional<unsigned> id_unexported_;
  std::optional<unsigned> id_exported_;
  PPCallbacks::FileIncludes &file_includes_;

  // Accumulates the set of declarations that have been marked for export by
  // this visitor.
  DeclSet exported_decls_;

  void add_missing_include(clang::SourceLocation location) {
    if (include_header.empty())
      return;

    clang::DiagnosticsEngine &diagnostics_engine = context_.getDiagnostics();

    static unsigned kID = diagnostics_engine.getCustomDiagID(
        clang::DiagnosticsEngine::Remark, "missing include statement %0");

    clang::SourceLocation spellingLoc =
        source_manager_.getSpellingLoc(location);
    const std::string fileName = source_manager_.getFilename(spellingLoc).str();
    auto &includes = file_includes_[fileName];

    // TODO: if the modified file contains no existing include directives, we
    // cannot currently determine where to insert the required include.
    if (includes.empty())
      return;

    // Determine if the header is already included.
    if (std::any_of(includes.begin(), includes.end(),
                    [](const PPCallbacks::IncludeLocation &include) {
                      return std::get<0>(include) == include_header;
                    }))
      return;

    // Insert the new include at the start of the existing include list. Rely
    // on clang-format to properly sort the include statements in alphabetical
    // order.
    clang::SourceLocation insertLoc =
        source_manager_.getSpellingLoc(std::get<1>(includes.front()));

    // Emit the fix-it hint to add the include statement.
    // TODO: consider using std::format after moving to C++20
    std::string FixText = "#include \"" + include_header + "\"\n";
    clang::FixItHint FixIt =
        clang::FixItHint::CreateInsertion(insertLoc, FixText);
    diagnostics_engine.Report(insertLoc, kID) << include_header << FixIt;

    // Add the new include to our list so we don't add it again.
    includes.insert(
        includes.begin(),
        std::tuple(static_cast<std::string>(include_header), insertLoc));
  }

  clang::DiagnosticBuilder
  unexported_public_interface(clang::SourceLocation location, const clang::Decl *D) {
    add_missing_include(location);
    exported_decls_.insert(D);

    clang::DiagnosticsEngine &diagnostics_engine = context_.getDiagnostics();

    if (!id_unexported_)
      id_unexported_ =
          diagnostics_engine.getCustomDiagID(clang::DiagnosticsEngine::Remark,
                                             "unexported public interface %0");

    return diagnostics_engine.Report(location, *id_unexported_);
  }

  clang::DiagnosticBuilder
  exported_private_interface(clang::SourceLocation location) {
    clang::DiagnosticsEngine &diagnostics_engine = context_.getDiagnostics();

    if (!id_exported_)
      id_exported_ =
          diagnostics_engine.getCustomDiagID(clang::DiagnosticsEngine::Remark,
                                             "exported private interface %0");

    return diagnostics_engine.Report(location, *id_exported_);
  }

  template <typename Decl_>
  inline clang::FullSourceLoc get_location(const Decl_ *TD) const {
    return context_.getFullLoc(TD->getBeginLoc()).getExpansionLoc();
  }

  template <typename Decl_>
  bool is_in_header(const Decl_ *D) const {
    const clang::FullSourceLoc location = get_location(D);
  const clang::FileID id = source_manager_.getFileID(location);
  if (const auto entry = source_manager_.getFileEntryRefForID(id)) {
    const llvm::StringRef name = entry->getName();
    for (const auto &extension : {".h", ".hh", ".hpp", ".hxx"})
      if (name.ends_with(extension))
        return true;
    }
    return false;
  }

  template <typename Decl_>
  inline bool is_in_system_header(const Decl_ *D) const {
    return source_manager_.isInSystemHeader(get_location(D));
  }

  template <typename Decl_>
  bool is_symbol_exported(const Decl_ *D) const {
    // Check the set of symbols we've already marked for export.
    if (exported_decls_.contains(D))
      return true;

    // Check if the symbol is annotated with __declspec(dllimport) or
    // __declspec(dllexport).
    if (D->template hasAttr<clang::DLLExportAttr>() ||
        D->template hasAttr<clang::DLLImportAttr>())
      return true;

    // Check if the symbol is annotated with [[gnu::visibility("default")]]
    // or the equivalent __attribute__((visibility("default")))
    if (const auto *VA = D->template getAttr<clang::VisibilityAttr>())
      return VA->getVisibility() == clang::VisibilityAttr::VisibilityType::Default;

    if (llvm::isa<clang::RecordDecl>(D))
      return false;

    // For non-record declarations, check to see if the containing record, if
    // any, is exported. This exports the member as well.
    for (const clang::DeclContext *DC = D->getDeclContext(); DC; DC = DC->getParent())
      if (const auto *RD = llvm::dyn_cast<clang::RecordDecl>(DC))
        return is_symbol_exported(RD);

    return false;
  }

  // Determine if a function needs exporting and add the export annotation as
  // required.
  void export_function_if_needed(const clang::FunctionDecl *FD) {
    // Check if the symbol is already exported.
    if (is_symbol_exported(FD))
      return;

    // Ignore declarations from the system.
    if (is_in_system_header(FD))
      return;

    // Skip declarations not in header files.
    if (!is_in_header(FD))
      return;

    // We are only interested in non-dependent types.
    if (FD->isDependentContext())
      return;

    // If the function has a body, it can be materialized by the user.
    if (FD->hasBody())
      return;

    // Skip methods in template declarations.
    if (FD->getTemplateInstantiationPattern() != nullptr)
      return;

    // Ignore friend declarations.
    if (FD->getFriendObjectKind() != clang::Decl::FOK_None)
      return;

    // Ignore deleted and defaulted functions (e.g. operators).
    if (FD->isDeleted() || FD->isDefaulted())
      return;

    // Skip template class template argument deductions.
    if (llvm::isa<clang::CXXDeductionGuideDecl>(FD))
      return;

    // Pure virtual methods cannot be exported.
    if (const auto *MD = llvm::dyn_cast<clang::CXXMethodDecl>(FD))
      if (MD->isPureVirtual())
        return;

    // Ignore known forward declarations (builtins)
    if (contains(kIgnoredBuiltins, FD->getNameAsString()))
      return;

    // TODO(compnerd) replace with std::set::contains in C++20
    if (contains(get_ignored_symbols(), FD->getNameAsString()))
      return;

    clang::SourceLocation SLoc =
        FD->getTemplatedKind() == clang::FunctionDecl::TK_NonTemplate
            ? FD->getBeginLoc()
            : FD->getInnerLocStart();
    unexported_public_interface(get_location(FD), FD)
        << FD << clang::FixItHint::CreateInsertion(SLoc, export_macro + " ");
  }

  // Determine if a variable needs exporting and add the export annotation as
  // required. This only applies to extern globals and static member fields.
  void export_variable_if_needed(const clang::VarDecl *VD) {
    // Check if the symbol is already exported.
    if (is_symbol_exported(VD))
      return;

    // Ignore declarations from the system.
    if (is_in_system_header(VD))
      return;

    // Skip local variables. We are only interested in static fields.
    if (VD->getParentFunctionOrMethod())
      return;

    // Skip static fields that have initializers.
    if (VD->hasInit())
      return;

    // Skip all variable declarations not in header files.
    if (!is_in_header(VD))
      return;

    // Skip all other local and global variables unless they are extern.
    if (!VD->isStaticDataMember() &&
        VD->getStorageClass() != clang::StorageClass::SC_Extern)
      return;

    // Skip fields in template declarations.
    if (VD->getTemplateInstantiationPattern() != nullptr)
      return;

    // Skip static variables declared in template class unless the template is
    // fully specialized.
    if (auto *RD = llvm::dyn_cast<clang::CXXRecordDecl>(VD->getDeclContext())) {
      if (RD->getDescribedClassTemplate())
        return;

      if (auto *CTSD = llvm::dyn_cast<clang::ClassTemplateSpecializationDecl>(RD))
        if (llvm::isa<clang::ClassTemplatePartialSpecializationDecl>(CTSD))
          return;
    }

    // TODO(compnerd) replace with std::set::contains in C++20
    if (contains(get_ignored_symbols(), VD->getNameAsString()))
      return;

    clang::SourceLocation SLoc = VD->getBeginLoc();
    unexported_public_interface(get_location(VD), VD)
        << VD << clang::FixItHint::CreateInsertion(SLoc, export_macro + " ");
  }

  // Determine if a tagged type needs exporting at the record level and add the
  // export annotation as required.
  void export_record_if_needed(clang::CXXRecordDecl *RD) {
    // Check if the class is already exported.
    if (is_symbol_exported(RD))
      return;

    // Ignore declarations from the system.
    if (is_in_system_header(RD))
      return;

    // Skip exporting template classes. For fully-specialized template classes,
    // isTemplated() returns false so they will be annotated if needed.
    if (RD->isTemplated())
      return;

    // If a class declaration contains an out-of-line virtual method, annotate
    // the class instead of its individual members. This ensures its vtable is
    // exported on non-Windows platforms. Do this regardless of the method's
    // access level.
    bool should_export_record = false;
    for (const auto *MD : RD->methods())
      if ((should_export_record =
               !(MD->isPureVirtual() || MD->isDefaulted() || MD->isDeleted()) &&
               (MD->isVirtual() && !MD->hasBody())))
        break;

    if (!should_export_record)
      return;

    // Insert the annotation immediately before the tag name, which is the
    // position returned by getLocation.
    clang::LangOptions LO = RD->getASTContext().getLangOpts();
    clang::SourceLocation SLoc = RD->getLocation();
    const clang::SourceLocation location =
        context_.getFullLoc(SLoc).getExpansionLoc();
    unexported_public_interface(location, RD)
        << RD << clang::FixItHint::CreateInsertion(SLoc, export_macro + " ");
  }

public:
  visitor(clang::ASTContext &context, PPCallbacks::FileIncludes &file_includes)
      : context_(context), source_manager_(context.getSourceManager()),
        file_includes_(file_includes) {}

  bool TraverseCXXRecordDecl(clang::CXXRecordDecl *RD) {
    export_record_if_needed(RD);

    // Traverse the class by invoking the parent's version of this method. This
    // call is required even if the record is exported because it may contain
    // nested records.
    return RecursiveASTVisitor::TraverseCXXRecordDecl(RD);
  }

  // RecursiveASTVisitor::TraverseCXXRecordDecl does not get called for fully
  // specialized template declarations. Since we may want to export them,
  // manually invoke TraverseCXXRecordDecl whenever an explicit specialization
  // is found.
  bool TraverseClassTemplateSpecializationDecl(
      clang::ClassTemplateSpecializationDecl *SD) {
    switch (SD->getSpecializationKind()) {
    case clang::TSK_ExplicitSpecialization:
      // This call visits class template specialization record and recursively
      // visits all of it children, which may also require export.
      return TraverseCXXRecordDecl(SD);

    // TODO: consider annotating explicit template instantiation declarations
    // and definitions in the future. They may require unique annotation macros
    // due to differences between visibility and dllexport/dllimport attributes.
    case clang::TSK_ExplicitInstantiationDeclaration:
      [[fallthrough]];
    case clang::TSK_ExplicitInstantiationDefinition:
      [[fallthrough]];
    default:
      return true;
    }
  }

  // VisitFunctionDecl will visit all function declarations. This includes top-
  // level functions as well as class member and static functions.
  bool VisitFunctionDecl(clang::FunctionDecl *FD) {
    // Ignore private member function declarations. Any that require export will
    // be identified by VisitCallExpr.
    if (const auto *MD = llvm::dyn_cast<clang::CXXMethodDecl>(FD))
      if (MD->getAccess() == clang::AccessSpecifier::AS_private)
        return true;

    export_function_if_needed(FD);
    return true;
  }

  // Visit every function call in the compilation unit to determine if there are
  // any inline calls to private member functions. In this uncommon case, the
  // private method must be annotated for export.
  bool VisitCallExpr(clang::CallExpr *CE) {
    const clang::FunctionDecl *FD = CE->getDirectCallee();
    if (!FD)
      return true;

    const clang::CXXMethodDecl *MD = llvm::dyn_cast<clang::CXXMethodDecl>(FD);
    if (!MD)
      return true;

    // Only consider private methods here. Non-private methods will be
    // considered for export by  VisitFunctionDecl.
    if (MD->getAccess() == clang::AccessSpecifier::AS_private)
      export_function_if_needed(MD);

    return true;
  }

  // VisitVarDecl will visit all variable declarations as well as static fields
  // in classes and structs. Non-static fields are not visited by this method.
  bool VisitVarDecl(clang::VarDecl *VD) {
    // Ignore private static field declarations. Any that require export will be
    // identified by VisitDeclRefExpr.
    if (VD->getAccess() == clang::AccessSpecifier::AS_private)
      return true;

    export_variable_if_needed(VD);
    return true;
  }

  // Visit every variable reference in the compilation unit to determine if
  // there are any inline references to private, static member fields. In this
  // uncommon case, the private field must be annotated for export.
  bool VisitDeclRefExpr(clang::DeclRefExpr *DRE) {
    // Only consider expresions referencing variable declarations. This includes
    // static fields and local variables but not member variables, which are
    // type FieldDecl.
    auto *VD = llvm::dyn_cast<clang::VarDecl>(DRE->getDecl());
    if (!VD)
      return true;

    // Only consider private fields here. Non-private fields will be considered
    // for export by VisitVarDecl.
    if (VD->getAccess() != clang::AccessSpecifier::AS_private)
      return true;

    export_variable_if_needed(VD);
    return true;
  }
};

class consumer : public clang::ASTConsumer {
  struct fixit_options : clang::FixItOptions {
    fixit_options() {
      InPlace = inplace;
      Silent = apply_fixits;
    }

    std::string RewriteFilename(const std::string &filename, int &fd) override {
      llvm_unreachable("unexpected call to RewriteFilename");
    }
  };

  idt::visitor visitor_;

  fixit_options options_;
  std::unique_ptr<clang::FixItRewriter> rewriter_;

public:
  consumer(clang::ASTContext &context, PPCallbacks::FileIncludes &file_includes)
      : visitor_(context, file_includes) {}

  void HandleTranslationUnit(clang::ASTContext &context) override {
    if (apply_fixits) {
      clang::DiagnosticsEngine &diagnostics_engine = context.getDiagnostics();
      rewriter_ =
          std::make_unique<clang::FixItRewriter>(diagnostics_engine,
                                                 context.getSourceManager(),
                                                 context.getLangOpts(),
                                                 &options_);
      diagnostics_engine.setClient(rewriter_.get(), /*ShouldOwnClient=*/false);
    }

    visitor_.TraverseDecl(context.getTranslationUnitDecl());

    if (apply_fixits)
      rewriter_->WriteFixedFiles();
  }
};

struct action : clang::ASTFrontendAction {
  void ExecuteAction() override {
    if (!include_header.empty())
      installPPCallbacks();
    clang::ASTFrontendAction::ExecuteAction();
  }

  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
    return std::make_unique<idt::consumer>(CI.getASTContext(), file_includes_);
  }

private:
  // Install a callback that will be invoked on every preprocessor include
  // statement. This is done so we can determine if a user-specified custom
  // include statment needs to be added if any annotations are added.
  void installPPCallbacks() {
    clang::CompilerInstance &compiler_instance = getCompilerInstance();
    clang::Preprocessor &preprocessor = compiler_instance.getPreprocessor();
    clang::SourceManager &source_manager = compiler_instance.getSourceManager();
    preprocessor.addPPCallbacks(
        std::make_unique<PPCallbacks>(source_manager, file_includes_));
  }

  PPCallbacks::FileIncludes file_includes_;
};

struct factory : clang::tooling::FrontendActionFactory {
  std::unique_ptr<clang::FrontendAction> create() override {
    return std::make_unique<idt::action>();
  }
};
}

int main(int argc, char *argv[]) {
  using namespace clang::tooling;

  auto options =
      CommonOptionsParser::create(argc, const_cast<const char **>(argv),
                                  idt::category, llvm::cl::OneOrMore);
  if (options) {
    ClangTool tool{options->getCompilations(), options->getSourcePathList()};
    return tool.run(new idt::factory{});
  } else {
    llvm::logAllUnhandledErrors(std::move(options.takeError()), llvm::errs());
    return EXIT_FAILURE;
  }
}
