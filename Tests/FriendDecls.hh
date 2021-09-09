// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// XFAIL: *

struct record {
  friend void friend_function();
// CHECK-NOT: FriendDecls.hh:[[@LINE-1]]:3: remark: unexported public interface 'friend_function'
};
