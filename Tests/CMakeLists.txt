
find_package(Python COMPONENTS Interpreter)
find_program(LIT_EXECUTABLE NAMES lit-script.py lit.py lit)
find_program(FILECHECK_EXECUTABLE NAMES FileCheck)

set(IDS_SRC_DIR ${PROJECT_SOURCE_DIR})
set(IDS_OBJ_DIR ${PROJECT_BINARY_DIR})
configure_file(lit.site.cfg.in lit.site.cfg @ONLY)

add_custom_target(check-ids
  COMMAND ${Python_EXECUTABLE} ${LIT_EXECUTABLE} -sv ${PROJECT_BINARY_DIR}/Tests --param idt=$<TARGET_FILE:idt>
  DEPENDS
    idt
    lit.cfg
    ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  COMMENT "Running ids tests..."
  USES_TERMINAL)
