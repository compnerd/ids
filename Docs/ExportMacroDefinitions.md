# Export Macro Definitions

A common approach for annotating project’s public interface is by defining a
set of preprocessor macros that are used instead of adding annotations directly
to the source. This approach allows supporting visibility annotations
(`[[gnu::visibility]]`, `__visibility__`) when building for Linux and MacOS and
DLL import/export annotations (`__declspec(dllimport)`,
`__declspec(dllexport)`) when building for Windows.

For example, the `PROJECT_EXPORT` macro defined below can be added to symbols in a
project’s header files files that should be externally visible.

```cpp
#if defined(PROJECT_EXPORT_STATIC)
/* We are building Project as a static library (no exports) */
#  define PROJECT_EXPORT
#else
/* We are building Project as a shared library */
#  if defined(_WIN32)
#    if defined(Project_EXPORTS)
       /* We are building this library */
#      define PROJECT_EXPORT __declspec(dllexport)
#    else
       /* We are using this library */
#      define PROJECT_EXPORT __declspec(dllimport)
#    endif
#  else
#    define PROJECT_EXPORT __visibility__((visibility("default")))
#  endif
#endif
```

A Windows project using these macros must be built with `Project_EXPORTS`
defined so that public symbols are annotated for export with
`__declspec(dllexport)`. When building clients of the project, however,
`Project_EXPORTS` must not be defined so the same set of public symbols will be
properly annotated for import with `__declspec(dllimport)`.

An ELF shared library or Mach-O dylib project will have all symbols publicly
visible by default so annotations are not strictly required. However, when a
project is built with default hidden symbol visibility using
`-fvisibility-default=hidden`, individual symbols must be explicitly exported
using a visibility annotations.

## CMake Support
When building with CMake, the `generate_export_header()` function can be used
to auto-generate a header with appropriate export macro definitions. See [CMake
documentation](https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html)
for details.
