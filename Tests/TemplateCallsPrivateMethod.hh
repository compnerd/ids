// RUN: %idt --extra-arg="-fno-delayed-template-parsing" --export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s

class TemplateCallsPrivateMethod {
public:
  // CHECK-NOT: TemplateCallsPrvateMethod.hh:[[@LINE+1]]:{{.*}}
  template <typename T> void publicTemplateMethod(T x) {
    privateMethodForTemplate(x);
  }

private:
  // NOTE: we use CHECK-DAG here because these remarks may come out of order and
  // we cannot control the order by rearranging members.

  // CHECK-DAG: TemplateCallsPrivateMethod.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethodForTemplate'
  void privateMethodForTemplate(long x) const;

  // CHECK-DAG: TemplateCallsPrivateMethod.hh:[[@LINE+1]]:3: remark: unexported public interface 'privateMethodForTemplate'
  void privateMethodForTemplate(int x) const;
};

