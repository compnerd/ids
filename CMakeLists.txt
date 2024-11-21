cmake_minimum_required(VERSION 3.20)
project(libtool
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_INLINES_HIDDEN yes)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Allow adding this project as a external project to the LLVM build system using LLVM_EXTERNAL_PROJECTS
if(PROJECT_IS_TOP_LEVEL)
  find_package(LLVM REQUIRED)
  message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
  message(STATUS "Using LLVMConfig.cmake in ${LLVM_DIR}")

  find_package(Clang REQUIRED)
  message(STATUS "Found Clang ${Clang_PACKAGE_VERSION}")
  message(STATUS "Using ClangConfig.cmake in ${Clang_DIR}")
else()
  set(CLANG_INCLUDE_DIRS
    "${LLVM_EXTERNAL_CLANG_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/tools/clang/include")
  set(LLVM_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include")
endif()

add_subdirectory(Sources)
add_subdirectory(Tests)
