#include "Lexer.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FormatVariadic.h"
#include <cstdint>
#include <string>

static bool isWhiteSpace(char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static bool isDigit(char c) {
  return '0' <= c && c <= '9';
}

static bool isLetter(char c) {
  return ('a' <= c && c <= 'z') ||
         ('A' <= c && c <= 'Z') || c == '_';
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

  // Aggregate the number characteristics into a number.
  if (isDigit(*BufPtr)) {
    int number = 0;
    tok.tokenType = TokenType::number;
    while (isDigit(*BufPtr)) {
      number = number*10 + (*BufPtr) - '0';
      BufPtr++; 
    }
    tok.value = number;
    tok.kind = CType::getIntTy();
    tok.content = llvm::StringRef(start, BufPtr-start);
    return;
  }

  if (isLetter(*BufPtr)) {
    while (isLetter(*BufPtr) || isDigit(*BufPtr)) {
      BufPtr++; 
    }
    
    llvm::StringRef content(start, BufPtr-start);
    if (content == "int") {
      tok.tokenType = TokenType::kw_int;
    }
    else {
      tok.tokenType = TokenType::identifier;
    }

    tok.content = content;
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
  case '=':
    tok.tokenType = TokenType::equal;
    BufPtr++;
    break;
  case ',':
    tok.tokenType = TokenType::comma;
    BufPtr++;
    break;
  default:
    tok.tokenType = TokenType::unknown;  
    BufPtr++;
  }
  tok.content = llvm::StringRef(start, BufPtr-start);
  return;  
}

void Lexer::saveState() {
  state.BufPtr = this->BufPtr;
  state.LineHeadPtr = this->LineHeadPtr;
  state.BufEnd = this->BufEnd;
  state.row = this->row;
}

void Lexer::restoreState() {
  this->BufPtr = state.BufPtr;
  this->LineHeadPtr = state.LineHeadPtr;
  this->BufEnd = state.BufEnd;
  this->row = state.row;
}