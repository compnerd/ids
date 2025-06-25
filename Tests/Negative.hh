// NOTE: We expect idt to return a non-zero exit code on Windows because the
// compiler will actually produce errors when __declspec(dllexport) appears in
// unsupported locations (not the case for visibility attributes). So we have
// to run "not idt" when running on Windows so the test doesn't fail due to
// the non-zero exit code from idt.

// RUN: %if system-windows %{ \
// RUN:   not %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s \
// RUN: %} %else %{  \
// RUN:   %idt --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s \
// RUN: %}

#if defined(_WIN32)
#define IDS_TEST_ABI __declspec(dllexport)
#else
#define IDS_TEST_ABI __attribute__((visibility("default")))
#endif

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'staticFunction'
IDS_TEST_ABI static void staticFunction() {}

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'inlineFunction'
IDS_TEST_ABI inline void inlineFunction() {}

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'function'
IDS_TEST_ABI void function() {}

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'constVariable'
IDS_TEST_ABI const int constVariable = 0;

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'staticConstVariable'
IDS_TEST_ABI static const int staticConstVariable = 0;

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'constexprVariable'
IDS_TEST_ABI constexpr int constexprVariable = 0;

// CHECK: Negative.hh:[[@LINE+1]]:1: remark: improperly exported symbol 'staticConstexprVariable'
IDS_TEST_ABI static constexpr int staticConstexprVariable = 0;

// CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
struct Struct {
  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'Struct'
  IDS_TEST_ABI Struct() = default;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'Struct'
  IDS_TEST_ABI Struct(const Struct&) = delete;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'operator='
  IDS_TEST_ABI void operator=(const Struct&) = delete;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticMethod'
  IDS_TEST_ABI static void staticMethod() {}

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'method'
  IDS_TEST_ABI void method() {}

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'pureVirtualMethod'
  IDS_TEST_ABI virtual void pureVirtualMethod() = 0;

  // CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
  IDS_TEST_ABI inline void inlineMethod() const;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticConstField'
  IDS_TEST_ABI static const int staticConstField = 0;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticConstexprField'
  IDS_TEST_ABI static constexpr int staticConstexprField = 0;
};

// CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
void Struct::inlineMethod() const {}

// CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
struct IDS_TEST_ABI ExportedStruct {
  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticMethod'
  IDS_TEST_ABI static void staticMethod();

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'method'
  IDS_TEST_ABI void method();

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticConstField'
  IDS_TEST_ABI static const int staticConstField;

  // CHECK: Negative.hh:[[@LINE+1]]:3: remark: improperly exported symbol 'staticField'
  IDS_TEST_ABI static int staticField;

  // CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
  IDS_TEST_ABI friend void friendMethod();
};

// CHECK-NOT: Negative.hh:[[@LINE+1]]:{{.*}}
IDS_TEST_ABI void friendMethod();
