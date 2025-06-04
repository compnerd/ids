// RUN: %idt --include-header="project/ExportDefs.h" -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s --check-prefix=CHECK-ADD-INCLUDE
// RUN: %idt -export-macro IDT_TEST_ABI %s 2>&1 | %FileCheck %s --check-prefix=CHECK-NO-INCLUDE

#pragma once

#include "MissingInclude.hh"
// CHECK-ADD-INCLUDE: MissingInclude.hh:[[@LINE-1]]:1: remark: missing include statement project/ExportDefs.h
// CHECK-ADD-INCLUDE-NOT: MissingInclude.hh:[[@LINE-2]]:1: remark: missing include statement project/ExportDefs.h
// CHECK-NO-INCLUDE-NOT: MissingInclude.hh:[[@LINE-3]]:1: remark: missing include statement project/ExportDefs.h

// Each of the following declarations should get annotated by IDS, but the new
// #include statement should only get added once.

void function();
// CHECK-ADD-INCLUDE: MissingInclude.hh:[[@LINE-1]]:1: remark: unexported public interface 'function'
// CHECK-NO-INCLUDE: MissingInclude.hh:[[@LINE-2]]:1: remark: unexported public interface 'function'

extern int variable;
// CHECK-ADD-INCLUDE: MissingInclude.hh:[[@LINE-1]]:8: remark: unexported public interface 'variable'
// CHECK-NO-INCLUDE: MissingInclude.hh:[[@LINE-2]]:8: remark: unexported public interface 'variable'
