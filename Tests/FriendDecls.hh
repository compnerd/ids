// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

void declared_friend_function();

struct record {
  friend void friend_function();
// CHECK-NOT: FriendDecls.hh:[[@LINE-1]]:{{.*}}

  friend void declared_friend_function();
// CHECK-NOT: FriendDecls.hh:[[@LINE-1]]:{{.*}}

  void non_friend_function();
// CHECK: FriendDecls.hh:[[@LINE-1]]:3: remark: unexported public interface 'non_friend_function'
};
