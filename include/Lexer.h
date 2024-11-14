#ifndef LEXER_H_
#define LEXER_H_

#include "Type.h"

#include "llvm/ADT/StringRef.h"
#include <cstdint>

enum class TokenType {
  identifier,
  kw_int, // 'int'
  number,
  plus,   // '+'
  minus,  // '-'
  star,   // '*'
  slash,  // '/'
  lparen, // '('
  rparen, // ')'
  semi,   // ';'
  equal,  // '='
  comma,  // ','
  eof,
  unknown
};

struct Token {
  Token() : row(-1), 
            col(-1), 
            tokenType(TokenType::unknown) {}
  void dump();

  uint32_t row, col;
  TokenType tokenType;
  int32_t value;
  llvm::StringRef content;
  CType *kind;
};

class Lexer {
public:
  Lexer(llvm::StringRef sourceCode);
  void nextToken(Token &tok);

private:
  const char *BufPtr;
  const char *LineHeadPtr;
  const char *BufEnd;
  uint32_t row;
};


#endif // LEXER_H_