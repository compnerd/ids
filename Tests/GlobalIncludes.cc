// RUN: rm -rf %t
// RUN: mkdir %t
// RUN: echo "[{\"directory\":\"%p\",\"command\":\"clang++ -c GlobalIncludes.cc -I%p/include\",\"file\":\"%t/GlobalIncludes.obj\"}]" | sed -e 's/\\/\\\\/g' > %t/compile_commands.json
// RUN: %idt -p %t -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s
#include <GlobalHeader.h>
// CHECK: [[PATH:(.*[\\\/])?include[\\\/]GlobalHeader\.h]]:1:1: remark: unexported public interface 'globalFunction'

static void testFunction() {
  globalFunction();
}
