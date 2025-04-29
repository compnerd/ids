// RUN: %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
class WithPrivateMethods {
public:
  // CHECK: PrivateMethods.hh:[[@LINE+1]]:3: remark: unexported public interface 'publicMethod'
  int publicMethod();

  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  inline int publicInlineMethod() {
    return privateMethod() + publicMethod();
  }

  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  static inline void publicStaticInlineMethod() {
    privateStaticMethod();
  }

private:
  // CHECK: PrivateMethods.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethod'
  int privateMethod();

  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  int privateInlineMethod() {
    return 0;
  }

  // CHECK: PrivateMethods.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateStaticMethod'
  static void privateStaticMethod();

  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  friend void friendFunction(const WithPrivateMethods &obj);

  // CHECK: PrivateMethods.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethodForFriend'
  void privateMethodForFriend() const;
};

// CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
inline void friendFunction(const WithPrivateMethods &obj) {
  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  return obj.privateMethodForFriend();
}

// CHECK: PrivateMethods.hh:[[@LINE+1]]:7: remark: unexported public interface 'VTableWithPrivateMethods'
class VTableWithPrivateMethods {
public:
  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  virtual int publicVirtualMethod();

  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  inline int protectedInlineMethod() {
    return privateMethod() + publicVirtualMethod();
  }

private:
  // CHECK-NOT: PrivateMethods.hh:[[@LINE+1]]:{{.*}}
  int privateMethod();
};
