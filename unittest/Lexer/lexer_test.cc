#include <gtest/gtest.h>
#include <utility>

#include "DiagEngine.h"
#include "Lexer.h"

#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"


/* This file will generate a executable file `lexer_test` 
 * for runing the tests defined in this file. 
 */

class LexerTest : public ::testing::Test {
public:
  void SetUp() override {
    static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
        llvm::MemoryBuffer::getFile("../testset/lexer01.c");
    if (!buf) {
      llvm::errs() << "Test input not existed\n";
      return;
    }

    llvm::SourceMgr mgr;
    DiagEngine diagEngine(mgr);

    mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

    lexer = new Lexer(mgr, diagEngine);
  }

  void TearDown() override {
    delete lexer;
  }

  Lexer *lexer;
};

TEST_F(LexerTest, NextToken) {
  // 正确集
  std::vector<Token> expectedVec;

  // 当前集
  std::vector<Token> curVec;
  Token tok;
  while (true) {
    lexer->nextToken(tok);
    if (tok.tokenType == TokenType::eof) break;
    curVec.push_back(tok);
  }

  ASSERT_EQ(expectedVec, curVec);
  for (int i = 0; i < expectedVec.size(); ++i) {
    const auto &expectedTok = expectedVec[i];
    const auto &curTok = curVec[i];

    EXPECT_EQ(expectedTok.tokenType, curTok.tokenType);
    EXPECT_EQ(expectedTok.row, curTok.row);
    EXPECT_EQ(expectedTok.col, curTok.col);
  }
}