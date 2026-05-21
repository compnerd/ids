// RUN: rm -rf %t && mkdir %t
// RUN: echo "[{\"directory\":\"%p\",\"command\":\"clang++ -c %s\",\"file\":\"%s\"}]" | sed -e 's/\\/\\\\/g' > %t/compile_commands.json
// RUN: %idt -p %t --main-file %s -export-macro IDT_TEST_ABI %S/include/MainFileMainTU.h 2>&1 | %FileCheck %s

// The outer header's decl is in the main TU and gets a fixit.
// CHECK:     MainFileMainTU.h:2:1: remark: unexported public interface 'outer_decl'
// The inner header is transitively included but is *not* the main file,
// so its decl must be skipped.
// CHECK-NOT: MainFileMainTUInner.h
