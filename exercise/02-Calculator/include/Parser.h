#ifndef EXEC2_PARSER_H_
#define EXEC2_PARSER_H_

#include "Lexer.h"
#include "AST.h"

class Parser {
public:
  Parser(Lexer &lexer) : lexer(lexer) {}

  std::shared_ptr<Program> parseProgram(); 

private:
  Lexer lexer;
  Token tok;

  std::shared_ptr<Expr> parseTerm();
  std::shared_ptr<Expr> parseFactor();
  std::shared_ptr<Expr> parseExpr();

  bool expect(TokenType tokenType);
  bool consume(TokenType tokenType);
  void advance();
};

#endif // EXEC2_PARSER_H_