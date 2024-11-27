#ifndef LEXER_H_
#define LEXER_H_

#include "Type.h"
#include "DiagEngine.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"
#include <cstdint>

enum class TokenType {
# define TOKEN(type, spelling) type,
# include "Token.h.inc"
  eof,
};

struct Token {
  void dump();
  static llvm::StringRef getSpellingText(TokenType tokenType);

  uint32_t row, col;
  TokenType tokenType;
  int32_t value; // save the number literal
  llvm::StringRef content;
  //char *ptr;
  //size_t len;
  CType *ty;
};

class Lexer {
public:
  Lexer(llvm::SourceMgr &mgr, DiagEngine &diagEngine)
      : mgr(mgr), diagEngine(diagEngine) {
    unsigned id = mgr.getMainFileID();
    llvm::StringRef mainSrc = mgr.getMemoryBuffer(id)->getBuffer();
    
    LineHeadPtr = mainSrc.begin();
    BufPtr = mainSrc.begin();
    BufEnd = mainSrc.end(); 
    row = 1;
  }

  void nextToken(Token &tok);

  // For LL(1) parser.
  void saveState(); 
  void restoreState();

  DiagEngine &getDiagEngine() const {
    return diagEngine;
  }

private:
  const char *BufPtr;
  const char *LineHeadPtr;
  const char *BufEnd;
  uint32_t row;

private:
  llvm::SourceMgr &mgr;
  DiagEngine &diagEngine;

private:
  struct State {
    const char *BufPtr;
    const char *LineHeadPtr;
    const char *BufEnd;
    uint32_t row;
  } state;
};


#endif // LEXER_H_