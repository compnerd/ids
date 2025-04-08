// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

__declspec(dllexport) void functionDLLExport();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__declspec(dllexport) extern unsigned variableDLLExport;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__declspec(dllimport) void functionDLLImport();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__declspec(dllimport) extern unsigned variableDLLImport;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("default"))) void functionAttributeVisibilityDefault();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("default"))) extern unsigned variableAttributeVisibilityDefault;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("hidden"))) void functionAttributeVisibilityHidden();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionAttributeVisibilityHidden'

[[gnu::visibility("default")]] void functionGnuVisibilityDefault();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

[[gnu::visibility("default")]] extern unsigned variableGnuVisibilityDefault;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

[[gnu::visibility("hidden")]] extern unsigned variableGnuVisibilityHidden;
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'variableGnuVisibilityHidden'
