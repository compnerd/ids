// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

#if defined(_WIN32)
#define EXPORT_ABI __declspec(dllexport)
#define IMPORT_ABI __declspec(dllimport)
#else
#define EXPORT_ABI __attribute__((visibility("default")))
#define IMPORT_ABI __attribute__((visibility("default")))
#endif

EXPORT_ABI void functionDLLExport1();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

void EXPORT_ABI functionDLLExport2();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

EXPORT_ABI extern unsigned variableDLLExport1;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

extern EXPORT_ABI unsigned variableDLLExport2;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

IMPORT_ABI void functionDLLImport1();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

void IMPORT_ABI functionDLLImport2();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

IMPORT_ABI extern unsigned variableDLLImport1;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

extern IMPORT_ABI unsigned variableDLLImport2;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("default"))) void functionAttributeVisibilityDefault1();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

void __attribute__((visibility("default"))) functionAttributeVisibilityDefault2();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("default"))) extern unsigned variableAttributeVisibilityDefault1;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

extern __attribute__((visibility("default"))) unsigned variableAttributeVisibilityDefault2;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

__attribute__((visibility("hidden"))) void functionAttributeVisibilityHidden1();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionAttributeVisibilityHidden1'

void __attribute__((visibility("hidden"))) functionAttributeVisibilityHidden2();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionAttributeVisibilityHidden2'

[[gnu::visibility("default")]] void functionGnuVisibilityDefault();
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

[[gnu::visibility("default")]] extern unsigned variableGnuVisibilityDefault;
// CHECK-NOT: Attributes.hh:[[@LINE-1]]:{{.*}}

[[gnu::visibility("hidden")]] extern unsigned variableGnuVisibilityHidden;
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'variableGnuVisibilityHidden'

[[deprecated]] extern unsigned variableDeprecated;
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'variableDeprecated'

[[deprecated]] void functionDeprecated();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionDeprecated'

__attribute__((unused)) extern unsigned variableUnused1;
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'variableUnused1'

extern __attribute__((unused)) unsigned variableUnused2;
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'variableUnused2'

__attribute__((unused)) void functionUnused1();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionUnused1'

void __attribute__((unused)) functionUnused2();
// CHECK: Attributes.hh:[[@LINE-1]]:{{.*}}: remark: unexported public interface 'functionUnused2'
