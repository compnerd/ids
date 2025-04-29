// RUN: %idt --export-macro=IDT_TEST_ABI %s 2>&1 | %FileCheck %s

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
struct ClassWithOutOfLineMethods {
  // CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
  ClassWithOutOfLineMethods();

  // CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
  ~ClassWithOutOfLineMethods();

  // CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
  void method();

  // CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
  inline void inlineMethod();
};

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
inline ClassWithOutOfLineMethods::ClassWithOutOfLineMethods() {}

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
inline ClassWithOutOfLineMethods::~ClassWithOutOfLineMethods() {}

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
void ClassWithOutOfLineMethods::method() {}

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
inline void ClassWithOutOfLineMethods::inlineMethod() {}

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
void method();

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
void method() {};

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
inline void inlineMethod();

// CHECK-NOT: OutOfLineInlineMethods.hh:[[@LINE+1]]:{{.*}}
inline void inlineMethod() {};
