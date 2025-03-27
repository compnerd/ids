// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

void testFunctionDecl();
// CHECK-NOT: SourceFile.cc:[[@LINE-1]]:{{.*}}
