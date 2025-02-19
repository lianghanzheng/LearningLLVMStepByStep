cmake_minimum_required(VERSION 3.18)

project(From-LLVM-to-C-Compiler)

set(C_STANDARD 99)
set(CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17")

set(TINYCC_VERSION_STRING "0.1")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Version.inc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Version.inc
)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Module configuration
find_package(LLVM REQUIRED CONFIG)
message("LLVM FOUND: ${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)
include(HandleLLVMOptions)

if (NOT ${LLVM_ENABLE_RTTI})
  # Handle '-DLLVM_ENABLE_RTTI' is OFF
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include path configuration
include_directories("${LLVM_BINARY_DIR}/include") 
include_directories("${LLVM_INCLUDE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_BINARY_DIR}/include")

# Linkage configuration
set(LLVM_LINK_COMPONENTS 
  ${LLVM_TARGETS_TO_BUILD}
  Analysis
  CodeGen
  Core
  IPO
  AggressiveInstCombine
  InstCombine
  Instrumentation
  MC
  MCParser
  ObjCARCOpts
  Option
  ScalarOpts
  Support
  TransformUtils
  Vectorize
  X86CodeGen
  CodeGenTypes
)

# Subdirectories
add_subdirectory(exercise)
add_subdirectory(lib)
#add_subdirectory(unittest)

# Binary output
add_llvm_executable(tinycc main.cc)
target_link_libraries(tinycc
  PRIVATE
  TinyCFrontend
)