// Copyright (c) 2021 Saleem Abdulrasool.  All Rights Reserved.
// SPDX-License-Identifier: BSD-3-Clause

#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Rewrite/Frontend/FixItRewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <cstdlib>
#include <iostream>
#include <set>
#include <string>

namespace libtool {
llvm::cl::OptionCategory category{"libtool options"};
}

namespace {
// TODO(compnerd) make this configurable via a configuration file or command line
const std::set<std::string> kIgnoredFunctions{
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
             llvm::cl::cat(libtool::category));

llvm::cl::opt<bool>
apply_fixits("apply-fixits", llvm::cl::init(false),
             llvm::cl::desc("Apply suggested changes to decorate interfaces"),
             llvm::cl::cat(libtool::category));

llvm::cl::opt<bool>
inplace("inplace", llvm::cl::init(false),
        llvm::cl::desc("Apply suggested changes in-place"),
        llvm::cl::cat(libtool::category));
}

namespace libtool {
enum class diagnostic : int {
  unexported_public_interface,
  exported_private_member,
};

class visitor : public clang::RecursiveASTVisitor<visitor> {
  clang::ASTContext &context_;
  clang::SourceManager &source_manager_;

  template <diagnostic id>
  clang::DiagnosticBuilder diagnose(clang::SourceLocation location);

  template <>
  clang::DiagnosticBuilder diagnose<diagnostic::unexported_public_interface>(clang::SourceLocation location) {
    clang::DiagnosticsEngine &diagnostics = context_.getDiagnostics();
    static unsigned id = diagnostics.getCustomDiagID(clang::DiagnosticsEngine::Remark, "unexported public interface %0");
    return diagnostics.Report(location, id);
  }

  template <>
  clang::DiagnosticBuilder diagnose<diagnostic::exported_private_member>(clang::SourceLocation location) {
    clang::DiagnosticsEngine &diagnostics = context_.getDiagnostics();
    static unsigned id = diagnostics.getCustomDiagID(clang::DiagnosticsEngine::Remark, "exported private interface %0");
    return diagnostics.Report(location, id);
  }

  template <typename Decl_>
  inline clang::FullSourceLoc get_location(const Decl_ *TD) const {
    return context_.getFullLoc(TD->getBeginLoc()).getExpansionLoc();
  }

public:
  explicit visitor(clang::ASTContext &context)
      : context_(context), source_manager_(context.getSourceManager()) {}

  bool VisitFunctionDecl(clang::FunctionDecl *FD) {
    clang::FullSourceLoc location = get_location(FD);

    // Ignore declarations from the system.
    if (source_manager_.isInSystemHeader(location))
      return true;

    // If the function has a body, it can be materialized by the user.
    if (FD->hasBody())
      return true;

    // Let `VisitCXXMethodDecl` handle `CXXMethodDecl`s
    if (llvm::isa<clang::CXXMethodDecl>(FD))
      return true;

    // Ignore friend declarations.
    if (llvm::isa<clang::FriendDecl>(FD))
      return true;

    // If the function has a dll-interface, it is properly annotated.
    if (FD->hasAttr<clang::DLLExportAttr>() ||
        FD->hasAttr<clang::DLLImportAttr>())
      return true;

    // Known Forward Declarations
    if (kIgnoredFunctions.find(FD->getNameAsString()) != kIgnoredFunctions.end())
      return true;

    diagnose<diagnostic::unexported_public_interface>(location) << FD
        << clang::FixItHint::CreateInsertion(FD->getBeginLoc(), export_macro + " ");
    return true;
  }

  bool VisitCXXMethodDecl(clang::CXXMethodDecl *MD) {
    clang::FullSourceLoc location = get_location(MD);

    // Ignore declarations from the system.
    if (source_manager_.isInSystemHeader(location))
      return true;

    // We are only interested in non-dependent types.
    if (MD->isDependentContext())
      return true;

    // If the method has a body, it can be materialized by the user.
    if (MD->hasBody())
      return true;

    // Ignore friend declarations.
    if (llvm::isa<clang::FriendDecl>(MD))
      return true;

    // Ignore deleted and defaulted members.
    if (MD->isDeleted() || MD->isDefaulted())
      return true;

    // Ignore private members (except for a negative check).
    if (MD->getAccess() == clang::AccessSpecifier::AS_private) {
      // Private methods should not be exported.
      if (MD->hasAttr<clang::DLLExportAttr>())
        diagnose<diagnostic::exported_private_member>(location) << MD;
      return true;
    }

    // Methods which are explicitly exported are properly annotated.
    if (MD->hasAttr<clang::DLLExportAttr>() ||
        MD->hasAttr<clang::DLLImportAttr>())
      return true;

    const clang::CXXRecordDecl *RD = MD->getParent()->getCanonicalDecl();

    diagnose<diagnostic::unexported_public_interface>(location) << MD
        << clang::FixItHint::CreateInsertion(MD->getBeginLoc(), export_macro + " ");
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

  libtool::visitor visitor_;

  fixit_options options_;
  std::unique_ptr<clang::FixItRewriter> rewriter_;

public:
  explicit consumer(clang::ASTContext &context)
      : visitor_(context) {}

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
  std::unique_ptr<clang::ASTConsumer>
  CreateASTConsumer(clang::CompilerInstance &CI, llvm::StringRef) override {
    return std::make_unique<libtool::consumer>(CI.getASTContext());
  }
};

struct factory : clang::tooling::FrontendActionFactory {
  std::unique_ptr<clang::FrontendAction> create() override {
    return std::make_unique<libtool::action>();
  }
};
}

int main(int argc, char *argv[]) {
  using namespace clang::tooling;

  auto options =
      CommonOptionsParser::create(argc, const_cast<const char **>(argv),
                                  libtool::category, llvm::cl::OneOrMore);
  if (options) {
    ClangTool tool{options->getCompilations(), options->getSourcePathList()};
    return tool.run(new libtool::factory{});
  } else {
    llvm::logAllUnhandledErrors(std::move(options.takeError()), llvm::errs());
    return EXIT_FAILURE;
  }
}