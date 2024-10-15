#ifndef EXEC2_LEXER_H_
#define EXEC2_LEXER_H_

#include "llvm/ADT/StringRef.h"
#include <cstdint>

enum class TokenType {
  number,
  plus,   // '+'
  minus,  // '-'
  star,   // '*'
  slash,  // '/'
  lparen, // '('
  rparen, // ')'
  semi,   // ';'
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


#endif // EXEC2_LEXER_H_