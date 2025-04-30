// RUN: %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
class WithPrivateMembers {
public:
  // CHECK: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'publicMethod'
  int publicMethod();

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  inline int publicInlineMethod() {
    if (privateMethod() > 0)
      return privateMethod() + publicMethod();

    privateStaticInlineMethod();

    return privateMethod() - publicMethod();
  }

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  static inline void publicStaticInlineMethod() {
    privateStaticField += 1;

    if (privateStaticField > 0)
      privateStaticMethod();

    privateStaticInlineMethod();
  }

private:
  // CHECK: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethod'
  int privateMethod();
  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethod'

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  int privateInlineMethod() {
    return 0;
  }

  // CHECK: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateStaticField'
  static int privateStaticField;
  // CHECK-NOT: PrivateMembers.hh:[[@LINE-1]]:3: remark: unexported public interface 'privateStaticField'

  // CHECK: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateStaticMethod'
  static void privateStaticMethod();

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  static void privateStaticInlineMethod() {}

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  friend void friendFunction(const WithPrivateMembers &obj);

  // CHECK: PrivateMembers.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethodForFriend'
  void privateMethodForFriend() const;
};

// CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
inline void friendFunction(const WithPrivateMembers &obj) {
  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  return obj.privateMethodForFriend();
}

// CHECK: PrivateMembers.hh:[[@LINE+1]]:7: remark: unexported public interface 'VTableWithPrivateMembers'
class VTableWithPrivateMembers {
public:
  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  virtual int publicVirtualMethod();

  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  inline int protectedInlineMethod() {
    return privateMethod() + publicVirtualMethod();
  }

private:
  // CHECK-NOT: PrivateMembers.hh:[[@LINE+1]]:{{.*}}
  int privateMethod();
};
