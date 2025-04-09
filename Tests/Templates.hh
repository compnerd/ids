// RUN: %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// Template struct. No members should be annotated for exort.
template <typename T1, typename T2>
struct TemplateStruct {
  static const unsigned const_value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  static unsigned value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  void method();
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}
};

// Partially-specialized template struct. No members should be annotated for
// export.
template <typename T1>
struct TemplateStruct<T1, unsigned> {
  static const unsigned const_value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  static unsigned value;
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}

  void method();
  // CHECK-NOT: Templates.hh:[[@LINE-1]]:{{.*}}
};

// Fully-specialized template struct. Members should be annotated for export as
// with any other struct.
template <>
struct TemplateStruct<void*, unsigned> {
  static const unsigned const_value;
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'const_value'

  static unsigned value;
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'value'

  void method();
  // CHECK: Templates.hh:[[@LINE-1]]:3: remark: unexported public interface 'method'
};
