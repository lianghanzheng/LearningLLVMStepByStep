set(EXECUTABLE_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH}/Exec02)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(lib)
add_subdirectory(tests)

add_llvm_executable(exprc main.cc
  DEPENDS
  CalcCore
)

target_link_libraries(exprc PRIVATE CalcCore)