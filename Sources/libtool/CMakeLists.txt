add_executable(libtool
  libtool.cc)
target_include_directories(libtool PRIVATE
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS})
target_link_libraries(libtool PRIVATE
  clangRewriteFrontend
  clangTooling)
