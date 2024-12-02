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

  std::shared_ptr<ASTNode> parseStmt();
  std::shared_ptr<ASTNode> parseBlockStmt();
  std::shared_ptr<ASTNode> parseDeclStmt();
  std::shared_ptr<ASTNode> parseExprStmt();
  std::shared_ptr<ASTNode> parseIfStmt();
  std::shared_ptr<ASTNode> parseExpr();
  std::shared_ptr<ASTNode> parseAssignExpr();
  std::shared_ptr<ASTNode> parseAddsubExpr();
  std::shared_ptr<ASTNode> parseMuldivExpr();
  std::shared_ptr<ASTNode> parsePrimaryExpr();

  bool expect(TokenType tokenType);
  bool consume(TokenType tokenType);
  void advance();
  DiagEngine &getDiagEngine() const;
};

#endif // PARSER_H_