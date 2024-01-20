// RUN: %idt -export-macro IDT_TEST_ABI -ignore foo,bar %s 2>&1 | %FileCheck %s

void foo() noexcept;
// CHECK-NOT: IgnoredFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'foo'

int bar(int x);
// CHECK-NOT: IgnoredFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'bar'

const char* snaf(int count);
// CHECK: IgnoredFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'snaf'
