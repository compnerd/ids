# Running IDS

There are a number of command-line arguments to IDS that dictate its behavior.

```bash
USAGE: idt [options] <source0> [... <sourceN>]

OPTIONS:

Generic Options:

  --help                                      - Display available options (--help-hidden for more)
  --help-list                                 - Display list of available options (--help-list-hidden for more)
  --version                                   - Display the version of this program

interface definition scanner options:

  --apply-fixits                              - Apply suggested changes to decorate interfaces
  --export-macro=<define>                     - The macro to decorate interfaces with
  --extra-arg=<string>                        - Additional argument to append to the compiler command line
  --extra-arg-before=<string>                 - Additional argument to prepend to the compiler command line
  --ignore=<function-name[,function-name...]> - Ignore one or more functions
  --include-header=<header>                   - Header required for export macro
  --inplace                                   - Apply suggested changes in-place
  -p <string>                                 - Build path
```

At a minimum, the `--export-macro` argument must be provided to specify the
macro used to annotate public symbols. See [Export Macro
Definitions](Docs/ExportMacroDefinitions.md) for details. Additionally, at
least one source file must be specified as a positional argument.

While it is possible to specify a number of source files, IDS generally works
better when invoked to process one file at a time.

## Windows Example

```powershell
S:\Source\ids\build\bin\idt.exe `
  -p S:\Source\MyProject\build `
  --apply-fixits `
  --inplace `
  --export-macro=PUBLIC_ABI `
  --include-header="include/MyAnnotations.h" `
  --extra-arg="-DPUBLIC_ABI=__declspec(dllexport)" `
  --extra-arg="-Wno-macro-redefined" `
  --extra-arg="-fno-delayed-template-parsing" `
  S:\Source\MyProject\include\ProjectHeader1.h `
  S:\Source\MyProject\include\ProjectHeader2.h
```

## Linux Example

```bash
/home/user/src/ids/out/bin/idt \
  -p /home/user/src/MyProject/build \
  --apply-fixits \
  --inplace \
  --export-macro=PUBLIC_ABI \
  --include-header="include/MyAnnotations.h" \
  --extra-arg="-DLLVM_ABI=[[gnu::visibility(\"default\")]]" \
  --extra-arg="-Wno-macro-redefined" \
  S:\Source\MyProject\include\ProjectHeader1.h \
  S:\Source\MyProject\include\ProjectHeader2.h
```

The arguments in the above examples have the following effects:
- `-p` refers to the build directory for the project containing a
  `compile_commands.json` file used by IDS to configure build options
- `--apply-fixits` and `--inplace` instruct IDS to modify the source file in
  place
- `--export-macro` indicates that the `PUBLIC_ABI` macro will be used to
  annotate public symbols
- `--include-header` specifies a local header file that will be added as a
  `#include` in the processed source files if needed. This would typically
  refer to the header file containing the export macro definition.
- The first two `--extra-arg` arguments ensure that `PUBLIC_ABI` is always
  defined (differently for Windows and Linux), and suppress the warning emitted
  if it already is. These arguments ensure the `PUBLIC_ABI` annotation is not
  mistakenly added to a symbol that already has it.
- The third `--extra-arg` argument is Windows-specific and ensures that
  templates are always expanded while parsing. It is ensures overloaded private
  methods get properly exported when referenced only from inline templates.
- The trailing positional arguments are the names of source files for IDS to
  scan.

