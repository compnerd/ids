// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename T, typename U> struct TemplateClass {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename U> struct TemplateClass<int, U> {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
extern template class TemplateClass<long, void*>;

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template class TemplateClass<unsigned, long>;

// CHECK: TemplateClasses.hh:[[@LINE+1]]:20: remark: unexported public interface 'TemplateClass<int, long>'
template <> struct TemplateClass<int, long> {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};
