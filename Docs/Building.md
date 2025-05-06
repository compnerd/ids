# Building IDS

IDS can be built either as an LLVM external project or as a stand-alone project.

## LLVM External Project

IDS can be built as a sub-project of LLVM by setting `LLVM_EXTERNAL_PROJECTS`.
This greatly improves the editing and debugging experience in Visual Studio when
navigating to Clang code from the IDS code. Please see [Getting Started with the
LLVM
Project](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
for more info on building LLVM and Clang.

The CMake variables `LLVM_EXTERNAL_PROJECTS` and `LLVM_EXTERNAL_IDS_SOURCE_DIR`
variables must be set for IDS to build as part of the LLVM build.

Testing IDS also requires `LIT_EXECUTABLE` and `FILECHECK_EXECUTABLE` to specify
the locations of `lit.py` and `FileCheck`, which are located in the LLVM source
and LLVM build output directory.

```bash
cmake -G Ninja \
  -B /home/user/src/llvm-project/build \
  -S /home/user/src/llvm-project/llvm \
  -DCMAKE_BUILD_TYPE=Release \
  -DLLVM_ENABLE_PROJECTS="llvm;clang" \
  -DLLVM_EXTERNAL_PROJECTS="IDS" \
  -DLLVM_EXTERNAL_IDS_SOURCE_DIR="/home/user/src/ids" \
  -DLIT_EXECUTABLE=/home/user/src/llvm-project/llvm/utils/lit/lit.py \
  -DFILECHECK_EXECUTABLE=/home/user/src/llvm-project/build/bin/FileCheck

cmake --build /home/user/src/llvm-project/build --target idt
cmake --build /home/user/src/llvm-project/build --target check-ids
```

## Stand-Alone Build

To build IDS as a stand-alone project, `LLVM_DIR` and `Clang_DIR` variables must
be provided to specify the locations of local LLVM and Clang libraries. These
can refer to locally built LLVM and Clang libraries, or to pre-built LLVM
package that includes the Clang development libraries.

Testing IDS also requires `LIT_EXECUTABLE` and `FILECHECK_EXECUTABLE` to specify
the locations of `lit.py` and `FileCheck` , which are not typically included in
a pre-built LLVM package.

```bash
cmake -G Ninja \
  -B /home/user/src/ids/build \
  -S home/user/src/ids \
  -DCMAKE_BUILD_TYPE=Release \
  -DLIT_EXECUTABLE=/home/user/src/llvm-project/llvm/utils/lit/lit.py \
  -DFILECHECK_EXECUTABLE=/home/user/src/llvm-project/build/bin/FileCheck \
  -DLLVM_DIR=/home/user/src/llvm-project/build/lib/cmake/llvm \
  -DClang_DIR=/home/user/src/llvm-project/build/lib/cmake/clang

cmake --build S:\ids\build --target idt
cmake --build S:\ids\build --target check-ids
```
