// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s
// %idt -export-macro IDT_LIBTOOL_TEST_ABI %s -fixit 2>/dev/null | %FileCheck %s -check-prefix CHECK-FIXIT

template <typename T> void template_function_inline(T &) { }
// CHECK-NOT: TemplateFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'template_function_inline'

template <> void template_function_inline<int>(int &) { }
// CHECK-NOT: TemplateFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'template_function_inline<int>'

template <> void template_function_inline<char>(char &);
// CHECK: TemplateFunctions.hh:[[@LINE-1]]:1: remark: unexported public interface 'template_function_inline<char>'
// CHECK-FIXIT: template <> IDT_TEST_ABI void template_function_inline<char>(char &);
