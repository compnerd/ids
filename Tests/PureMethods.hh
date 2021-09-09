// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

struct pure_virtual {
  virtual void pure_virtual_method() = 0;
// CHECK-NOT: PureMethods.hh:[[@LINE-1]]:3: remark: unexported public interface 'pure_virtual_method'
};

