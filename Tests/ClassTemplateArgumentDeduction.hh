// RUN: %idt --export-macro IDT_TEST_ABI --extra-arg="--std=c++17" %s 2>&1 | %FileCheck %s

template <typename T>
struct TemplateStruct {};

TemplateStruct(unsigned) -> TemplateStruct<unsigned>;
// CHECK-NOT: ClassTemplateArgumentDeduction.hh:[[@LINE-1]]:{{.*}}
