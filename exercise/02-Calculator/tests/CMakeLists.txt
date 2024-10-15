add_llvm_executable(lexer-test LexerTest.cc
  DEPENDS
  CalcCore
)

target_link_libraries(lexer-test PRIVATE CalcCore)