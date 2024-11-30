enable_testing()

add_executable(lexer_test
  lexer_test.cc

  ../../lib/Lexer.cc
  ../../lib/Type.cc
  ../../lib/DiagEngine.cc
)

llvm_map_components_to_libnames(llvm_all
  Support
  Core
)

message(STATUS "Linked libraries for test: ${llvm_all}")

target_link_libraries(lexer_test
  GTest::gtest_main
  ${llvm_all}
)

include(GoogleTest)
gtest_discover_tests(lexer_test)