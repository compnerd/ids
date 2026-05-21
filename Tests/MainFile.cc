// RUN: rm -rf %t && mkdir %t
// RUN: echo "[{\"directory\":\"%p\",\"command\":\"clang++ -c %s\",\"file\":\"%s\"}]" | sed -e 's/\\/\\\\/g' > %t/compile_commands.json
// RUN: %idt -p %t --main-file %s -export-macro IDT_TEST_ABI %S/include/MainFile.h 2>&1 | %FileCheck %s

// CHECK: MainFile.h:1:1: remark: unexported public interface 'main_file_target'
