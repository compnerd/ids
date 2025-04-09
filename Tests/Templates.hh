// RUN: %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

template <typename T>
struct TemplateStruct {
  static const unsigned const_value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  static unsigned value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  void method();
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}
};

template <>
struct TemplateStruct<unsigned> {
  static const unsigned const_value;
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'const_value'

  static unsigned value;
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'value'

  void method();
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'method'
};
