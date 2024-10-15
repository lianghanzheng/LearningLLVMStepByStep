cmake_minimum_required(VERSION 3.18)

project(From-LLVM-to-C-Compiler)

set(C_STANDARD 99)
set(CXX_STANDARD 17)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Module configuration
find_package(LLVM REQUIRED CONFIG)
message("LLVM FOUND: ${LLVM_CMAKE_DIR}")
list(APPEND CMAKE_MODULE_PATH ${LLVM_CMAKE_DIR})
include(AddLLVM)

if (NOT ${LLVM_ENABLE_RTTI})
  # Handle '-DLLVM_ENABLE_RTTI' is OFF
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include path configuration
include_directories("${LLVM_BINARY_DIR}/include") 
include_directories("${LLVM_INCLUDE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/include")

# Linkage configuration
set(LLVM_LINK_COMPONENTS Support Core)

# Binary output
add_llvm_executable(tinycc main.cc)

# Subdirectories
add_subdirectory(exercise)