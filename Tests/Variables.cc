// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

extern int extern_variable;
// CHECK-NOT: Variables.cc:[[@LINE-1]]:{{.*}}

extern const int extern_const_variable;
// CHECK-NOT: Variables.cc:[[@LINE-1]]:{{.*}}

