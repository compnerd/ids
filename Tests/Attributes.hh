// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

#if defined(_WIN32)
#define EXPORT_ABI __declspec(dllexport)
#define IMPORT_ABI __declspec(dllimport)
#else
#define EXPORT_ABI __attribute__((visibility("default")))
#define IMPORT_ABI __attribute__((visibility("default")))
#endif

EXPORT_ABI void functionDLLExport();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

EXPORT_ABI extern unsigned variableDLLExport;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

IMPORT_ABI void functionDLLImport();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

IMPORT_ABI extern unsigned variableDLLImport;
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
