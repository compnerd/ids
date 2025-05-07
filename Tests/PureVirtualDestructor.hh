// RUN: %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// PureVirtualClass should get annotated because it contains a pure virtual
// destructor with an out-of-line definition.

// CHECK: PureVirtualDestructor.hh:[[@LINE+1]]:8: remark: unexported public interface 'PureVirtualClass'
struct PureVirtualClass {
// CHECK-NOT: PureVirtualDestructor.hh:[[@LINE-1]]:{{.*}}

  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual ~PureVirtualClass() = 0;
  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE-1]]:{{.*}}

  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod() = 0;
};

// AnotherPureVirtualClass should get NOT get annotated because its pure virtual
// destructor has an out-of-line definition within this translation unit.

// CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
struct AnotherPureVirtualClass {
  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual ~AnotherPureVirtualClass() = 0;

  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod() = 0;
};

// CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
AnotherPureVirtualClass::~AnotherPureVirtualClass() {}

// YetAnotherPureVirtualClass should get NOT get annotated because its pure
// virtual destructor has an out-of-line defaulted definition within this
// translation unit.

// CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
struct YetAnotherPureVirtualClass {
  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual ~YetAnotherPureVirtualClass() = 0;

  // CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod() = 0;
};

// CHECK-NOT: PureVirtualDestructor.hh:[[@LINE+1]]:{{.*}}
YetAnotherPureVirtualClass::~YetAnotherPureVirtualClass() = default;
