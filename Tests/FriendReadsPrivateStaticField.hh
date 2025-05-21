// RUN: %idt --friendly-fields --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

struct KeyType {};

template <typename T> struct FriendTemplate {
  static KeyType *getKey() { return &T::Key; }
};

class ClassWithPrivateStaticField : public FriendTemplate<ClassWithPrivateStaticField> {
  friend FriendTemplate<ClassWithPrivateStaticField>;

  // CHECK: FriendReadsPrivateStaticField.hh:[[@LINE+1]]:3: remark: unexported public interface 'Key'
  static KeyType Key;
};
