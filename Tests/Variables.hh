// RUN: %idt -ignore ignored_extern_variable -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

class ClassWithFields {
public:
  int public_class_field;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

  static int public_static_class_field;
  // CHECK: Variables.hh:[[@LINE-1]]:3: remark: unexported public interface 'public_static_class_field'

  static const int public_static_const_class_field;
  // CHECK: Variables.hh:[[@LINE-1]]:3: remark: unexported public interface 'public_static_const_class_field'

  static const int public_static_const_init_class_field = 0;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

  static constexpr int public_static_constexpr_init_class_field = 0;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

private:
  static int private_static_class_field;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}
};

struct StructWithFields {
  int public_struct_field;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

  static int public_static_struct_field;
  // CHECK: Variables.hh:[[@LINE-1]]:3: remark: unexported public interface 'public_static_struct_field'

  static const int public_static_const_struct_field;
  // CHECK: Variables.hh:[[@LINE-1]]:3: remark: unexported public interface 'public_static_const_struct_field'

  static const int public_static_const_init_struct_field = 0;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

  static constexpr int public_static_constexpr_init_struct_field = 0;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

private:
  static int private_static_struct_field;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}
};

extern int extern_variable;
// CHECK: Variables.hh:[[@LINE-1]]:8: remark: unexported public interface 'extern_variable'

extern const int extern_const_variable;
// CHECK: Variables.hh:[[@LINE-1]]:14: remark: unexported public interface 'extern_const_variable'

const extern int const_extern_variable;
// CHECK: Variables.hh:[[@LINE-1]]:14: remark: unexported public interface 'const_extern_variable'

extern int ignored_extern_variable;
// CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

int global_variable;
// CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}

void function() {
  extern int extern_local_variable;
  // CHECK: Variables.hh:[[@LINE-1]]:10: remark: unexported public interface 'extern_local_variable'

  int local_variable;
  // CHECK-NOT: Variables.hh:[[@LINE-1]]:{{.*}}
}

extern StructWithFields* extern_struct_pointer;
// CHECK:Variables.hh:[[@LINE-1]]:8: remark: unexported public interface 'extern_struct_pointer'

extern const StructWithFields* extern_immutable_struct_pointer;
// CHECK:Variables.hh:[[@LINE-1]]:14: remark: unexported public interface 'extern_immutable_struct_pointer'

[[deprecated("do not use")]]extern int extern_cpp_deprecated_int;
// CHECK:Variables.hh:[[@LINE-1]]:36: remark: unexported public interface 'extern_cpp_deprecated_int'

__attribute((deprecated("do not use"))) extern int extern_deprecated_int;
// CHECK:Variables.hh:[[@LINE-1]]:48: remark: unexported public interface 'extern_deprecated_int'

__attribute((deprecated("do not use")))extern
int extern_deprecated_int_2_line;
// CHECK:Variables.hh:[[@LINE-1]]:1: remark: unexported public interface 'extern_deprecated_int_2_line'

[[deprecated("do not use")]] /* comment */extern int extern_cpp_deprecated_int_comment;
// CHECK:Variables.hh:[[@LINE-1]]:50: remark: unexported public interface 'extern_cpp_deprecated_int_comment'

extern volatile unsigned long *extern_volatile_unsigned_long_ptr;
// CHECK:Variables.hh:[[@LINE-1]]:17: remark: unexported public interface 'extern_volatile_unsigned_long_ptr'

extern unsigned long extern_unsigned_long_aligned [[gnu::aligned(16)]];
// CHECK:Variables.hh:[[@LINE-1]]:8: remark: unexported public interface 'extern_unsigned_long_aligned'
