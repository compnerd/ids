// RUN: %idt -ignore foo,bar %s 2>&1 | %FileCheck %s
// RUN: %idt -ignore foo -ignore bar %s 2>&1 | %FileCheck %s

void f() noexcept;
// CHECK-NOT: IgnoredFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'f'

int g(int x);
// CHECK-NOT: IgnoredFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'g'

const char* h(int count);
// CHECK: IgnoredFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'h'
