// RUN: %idt @%p/../Support/ResponseFiles/ignored-builtins.rsp -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

unsigned char _BitScanForward(unsigned long *Index, unsigned long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:{{.*}}

unsigned char _BitScanForward64(unsigned long *Index, unsigned long long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:{{.*}}

unsigned char _BitScanReverse(unsigned long *Index, unsigned long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:{{.*}}

unsigned char _BitScanReverse64(unsigned long *Index, unsigned long long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:{{.*}}

__SIZE_TYPE__ __builtin_strlen(const char *);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:{{.*}}
