// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

struct record {
  record() = default;
// CHECK-NOT: DefaultDeletedFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'record()'

  record(const record &) = delete;
// CHECK-NOT: DefaultDeletedFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'record(const record &)'
};

bool operator==(record &, record &) = delete;
// CHECK-NOT: DefaultDeletedFunctions.hh:[[@LINE-1]]:3: remark: unexported public interface 'operator=='
