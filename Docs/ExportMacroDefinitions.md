## Export Macro Definitions

A common approach for annotating project’s public interface is by defining a
set of preprocessor macros that are used instead of adding annotations directly
to the source. This approach allows supporting visibility annotations
(`[[gnu::visibility]]`, `__visibility__`) when building for Linux and MacOS and
DLL import/export annotations (`__declspec(dllimport)`,
`__declspec(dllexport)`) when building for Windows.

For example, the `PUBLIC_ABI` macro defined below can be added to symbols in a
project’s header files files that should be externally visible.

```cpp
#ifdef _WIN32
#ifdef EXPORTING_ABI
// When building the Windows DLL, the public API must be annotated for exported.
#define PUBLIC_ABI __declspec(dllexport)
#else
// When building clients of the Windows DLL, the public API must be annotated for import.
#define PUBLIC_ABI __declspec(dllimport)
#endif
#elif defined(__cplusplus) && defined(__has_cpp_attribute) && \
      __has_cpp_attribute(gnu::visibility) && defined(__GNUC__) && !defined(__clang__)
// When supported, use the gnu::visibility C++ attribute to set visibility to default.
#define PUBLIC_API [[gnu::visibility("default")]]
#elif defined(__has_attribute) && __has_attribute(visibility)
// Otherwise, use the __attribute__ to set visibility to default.
#define PUBLIC_ABI __attribute__((visibility("default")))
#else
#error "Required visibility attribute are not supported by compiler!"
#endif
```

A Windows project using these macros must be built with `EXPORTING_ABI` defined
so that public symbols are annotated for export with `__declspec(dllexport)`.
When building the client, however, `EXPORTING_ABI` must not be defined so the
same set of public symbols will be properly annotated for import with
`__declspec(dllimport)`.

An ELF shared library or Mach-O dylib project will have all symbols publicly
visible by default so annotations are not strictly required. However, when a
project is built with default hidden symbol visibility using
`-fvisibility-default=hidden`, individual symbols must be explicitly exported
using a visibility annotations.
