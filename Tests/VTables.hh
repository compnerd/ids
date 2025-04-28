// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct ClassWithInlineVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMedhod() {}
  // CHECK: VTables.hh:[[@LINE+1]]:3: remark: unexported public interface 'method'
  void method();
  // CHECK: VTables.hh:[[@LINE+1]]:3: remark: unexported public interface 'static_field'
  static unsigned static_field;
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct ClassWithDefaultVirtualDestructor {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual ~ClassWithDefaultVirtualDestructor() = default;
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct ClassWithDeletedVirtualDestructor {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual ~ClassWithDeletedVirtualDestructor() = delete;
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct ClassWithPureVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod() = 0;
};

// CHECK: VTables.hh:[[@LINE+1]]:8: remark: unexported public interface 'ClassWithExternVirtualMethod'
struct ClassWithExternVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void virtualMethod();
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  void method();
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct DerivedClassWithInlineOverriddenExternVirtualMethod : public ClassWithExternVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  void virtualMethod() override {}
};

// CHECK: VTables.hh:[[@LINE+1]]:8: remark: unexported public interface 'DerivedClassWithExternOverriddenExternVirtualMethod'
struct DerivedClassWithExternOverriddenExternVirtualMethod : public ClassWithExternVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  void virtualMethod() override;
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct DerivedClassWithInlineOverriddenPureVirtualMethod : public ClassWithPureVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  void virtualMethod() override {}
};

// CHECK: VTables.hh:[[@LINE+1]]:8: remark: unexported public interface 'DerivedClassWithExternOverriddenPureVirtualMethod'
struct DerivedClassWithExternOverriddenPureVirtualMethod : public ClassWithPureVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  void virtualMethod() override;
};

// CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
struct OuterClass {
  // CHECK: VTables.hh:[[@LINE+1]]:3: remark: unexported public interface 'method'
  void method();
  // CHECK: VTables.hh:[[@LINE+1]]:9: remark: unexported public interface 'InnerClassWithExternVirtualMethod'
  class InnerClassWithExternVirtualMethod {
  protected:
    // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
    virtual void virtualMethod();
    // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
    void method();
    // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
    static unsigned static_field;
  };

  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  struct InnerClassWithInlineVirtualMethod {
    // CHECK: VTables.hh:[[@LINE+1]]:5: remark: unexported public interface 'method'
    void method();
    // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
    virtual void virtualMedhod() {}
    // CHECK: VTables.hh:[[@LINE+1]]:12: remark: unexported public interface 'InnerInnerClassWithExternVirtualMethod'
    struct InnerInnerClassWithExternVirtualMethod {
      // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
      virtual void virtualMethod();
      // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
      void method();
    };
    // CHECK: VTables.hh:[[@LINE+1]]:5: remark: unexported public interface 'static_field'
    static unsigned static_field;
  };
  // CHECK: VTables.hh:[[@LINE+1]]:3: remark: unexported public interface 'static_field'
  static unsigned static_field;
};

// CHECK: VTables.hh:[[@LINE+1]]:22: remark: unexported public interface 'NoDiscardAttributeClass'
struct [[nodiscard]] NoDiscardAttributeClass {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  static unsigned static_field;
};

// CHECK: VTables.hh:[[@LINE+3]]:1: remark: unexported public interface 'NoDiscardAttributeMultiLineDefClass'
struct
[[nodiscard]]
NoDiscardAttributeMultiLineDefClass {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  static unsigned static_field;
};

// CHECK: VTables.hh:[[@LINE+1]]:32: remark: unexported public interface 'UnusedAttributeClass'
struct __attribute__((unused)) UnusedAttributeClass {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  static unsigned static_field;
};

// CHECK: VTables.hh:[[@LINE+3]]:1: remark: unexported public interface 'UnusedAttributeMultiLineDefClass'
struct
__attribute__((unused))
UnusedAttributeMultiLineDefClass {
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  virtual void method();
  // CHECK-NOT: VTables.hh:[[@LINE+1]]:{{.*}}
  static unsigned static_field;
};
