#ifndef PARSER_H_
#define PARSER_H_

#include "Lexer.h"
#include "AST.h"
#include "Sema.h"

class Parser {
public:
  Parser(Lexer &lexer, Sema &sema) 
      : lexer(lexer), sema(sema) {}

  std::shared_ptr<Program> parseProgram(); 

private:
  Lexer lexer;
  Token tok;
  Sema sema;

  std::vector<std::shared_ptr<ASTNode>> parseDecl();
  std::shared_ptr<ASTNode> parseTerm();
  std::shared_ptr<ASTNode> parseFactor();
  std::shared_ptr<ASTNode> parseExpr();

  bool expect(TokenType tokenType);
  bool consume(TokenType tokenType);
  void advance();
};

#endif // PARSER_H_