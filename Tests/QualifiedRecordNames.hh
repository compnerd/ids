// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

namespace ContainerNamespace {
  // CHECK-NOT: QualifiedRecordNames.hh:[[@LINE+1]]:{{.*}}
  struct QualifiedNameClass;
}

// CHECK: QualifiedRecordNames.hh:[[@LINE+1]]:8: remark: unexported public interface 'QualifiedNameClass'
struct ContainerNamespace::QualifiedNameClass {
// CHECK-NOT: QualifiedRecordNames.hh:[[@LINE-1]]:{{.*}}

  // CHECK-NOT: QualifiedRecordNames.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
};

// CHECK-NOT: QualifiedRecordNames.hh:[[@LINE+1]]:{{.*}}
struct ContainerClass {
  // CHECK-NOT: QualifiedRecordNames.hh:[[@LINE+1]]:{{.*}}
  struct QualifiedNameClass;
};

// CHECK: QualifiedRecordNames.hh:[[@LINE+1]]:8: remark: unexported public interface 'QualifiedNameClass'
struct ContainerClass::QualifiedNameClass {
// CHECK-NOT: QualifiedRecordNames.hh:[[@LINE-1]]:{{.*}}

  // CHECK-NOT: QualifiedRecordNames.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
};

