# Interface Definition Scanner (IDS)

IDS is tool for automatically identifying and annotating the public interface of
a C++ project. It is built on Clang’s
[LibTooling](https://clang.llvm.org/docs/LibTooling.html) framework. It is
primarily designed for adding import/export annotations to a Windows DLL
project, but can also be used to add visibility annotations for ELF shared
library or Mach-O dylib projects.

## Documentation
- [Building IDS](Docs/Building.md)
- [Running IDS](Docs/Running.md)
- [Export Macro Definitions](Docs/ExportMacroDefinitions.md)
