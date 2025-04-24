// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

struct ClassWithInlineVirtualMethod {
// CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
  virtual void virtualMedhod() {}
  // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
  void method();
  // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'method'
  static unsigned static_field;
  // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'static_field'
};

struct ClassWithExternVirtualMethod {
// CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'ClassWithExternVirtualMethod'
private:
  virtual void virtualMedhod();
  // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
  void method();
  // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
};

struct OuterClass {
// CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
  void method();
  // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'method'
  class InnerClassWithExternVirtualMethod {
  // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'InnerClassWithExternVirtualMethod'
  protected:
    virtual void virtualMethod();
    // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
    void method();
    // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
    static unsigned static_field;
    // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
  };

  struct InnerClassWithInlineVirtualMethod {
  // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
    void method();
    // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'method'
    virtual void virtualMedhod() {}
    // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
    struct InnerInnerClassWithExternVirtualMethod {
    // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'InnerInnerClassWithExternVirtualMethod'
      virtual void virtualMethod();
      // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
      void method();
      // CHECK-NOT: VTables.hh:[[@LINE-1]]:{{.*}}
    };

    static unsigned static_field;
    // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'static_field'
  };

  static unsigned static_field;
  // CHECK: VTables.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'static_field'
};
