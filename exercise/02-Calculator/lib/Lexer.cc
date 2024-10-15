#include "Lexer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormatVariadic.h"
#include <cstdint>

static bool isWhiteSpace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static bool isDigit(char c) {
  return '0' <= c && c <= '9';
}

void Token::dump() {
  llvm::errs() << llvm::formatv(
      "[ \"{0}\": row = {1}, col = {2} ]\n", 
      content, row, col);
}

Lexer::Lexer(llvm::StringRef sourceCode)
    : LineHeadPtr(sourceCode.begin()),
      BufPtr(sourceCode.begin()),
      BufEnd(sourceCode.end()), row(1) {}

void Lexer::nextToken(Token &tok) {
  // Filter the whitespaces.
  while (isWhiteSpace(*BufPtr)) {
    if (*BufPtr == '\n') {
      row++;
      LineHeadPtr = BufPtr + 1;
    }
    BufPtr++;
  }

  tok.row = row;
  tok.col = BufPtr - LineHeadPtr + 1;

  // Check whether we reach the eod of file.
  if (BufPtr >= BufEnd) {
    tok.tokenType = TokenType::eof;
    tok.content = "";
    return;
  }

  const char *start = BufPtr;
  uint32_t len = 0;

  // Aggregate the number characteristics into a number.
  if (isDigit(*BufPtr)) {
    int number = 0;
    tok.tokenType = TokenType::number;
    while (isDigit(*BufPtr)) {
      number = number*10 + (*BufPtr) - '0';
      BufPtr++; len++;
    }
    tok.value = number;
    tok.content = llvm::StringRef(start, len);
    return;
  }

  // Check the remained cases: symbols or invalid.
  switch (*BufPtr) {
  case '+':
    tok.tokenType = TokenType::plus;
    BufPtr++;
    break;
  case '-':
    tok.tokenType = TokenType::minus;
    BufPtr++;
    break;
  case '*':
    tok.tokenType = TokenType::star;
    BufPtr++;
    break;
  case '/':
    tok.tokenType = TokenType::slash;
    BufPtr++;
    break;
  case '(':
    tok.tokenType = TokenType::lparen;
    BufPtr++;
    break;
  case ')':
    tok.tokenType = TokenType::rparen;
    BufPtr++;
    break;
  case ';':
    tok.tokenType = TokenType::semi;
    BufPtr++;
    break;
  default:
    tok.tokenType = TokenType::unknown;  
    BufPtr++;
  }
  len += 1;
  tok.content = llvm::StringRef(start, len);
  return;  
}
