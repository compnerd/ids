// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename T, typename U> struct TemplateClassVirtual {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename U> struct TemplateClassVirtual<int, U> {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
extern template class TemplateClassVirtual<long, void*>;

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template class TemplateClassVirtual<unsigned, long>;

// CHECK: TemplateClasses.hh:[[@LINE+1]]:20: remark: unexported public interface 'TemplateClassVirtual<int, long>'
template <> struct TemplateClassVirtual<int, long> {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename T, typename U> struct TemplateClass {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  void method();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <typename U> struct TemplateClass<int, U> {
  // CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
  void method();
};

// CHECK-NOT: TemplateClasses.hh:[[@LINE+1]]:{{.*}}
template <> struct TemplateClass<int, long> {
  // CHECK: TemplateClasses.hh:[[@LINE+1]]:3: remark: unexported public interface 'method'
  void method();
};
