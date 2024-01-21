// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

unsigned char _BitScanForward(unsigned long *Index, unsigned long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:3: remark: unexported public interface '_BitScanForward'

unsigned char _BitScanForward64(unsigned long *Index, unsigned long long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:3: remark: unexported public interface '_BitScanForward64'

unsigned char _BitScanReverse(unsigned long *Index, unsigned long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:3: remark: unexported public interface '_BitScanReverse'

unsigned char _BitScanReverse64(unsigned long *Index, unsigned long long Mask);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:3: remark: unexported public interface '_BitScanReverse64'

__SIZE_TYPE__ __builtin_strlen(const char *);
// CHECK-NOT: KnownBuiltins.hh:[[@LINE-1]]:3: remark: unexported public interface '__builtin_strlen'
