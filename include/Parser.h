#ifndef PARSER_H_
#define PARSER_H_

#include "Lexer.h"
#include "AST.h"
#include "Sema.h"
#include <memory>
#include <vector>

class Parser {
public:
  Parser(Lexer &lexer, Sema &sema) 
      : lexer(lexer), sema(sema) {}

  std::shared_ptr<Program> parseProgram(); 

private:
  Lexer lexer;
  Token tok;
  Sema sema;

private:
  // Record the precursor of break and continue statements.
  std::vector<std::shared_ptr<ASTNode>> breakableStmts;
  std::vector<std::shared_ptr<ASTNode>> continableStmts;

private:
  std::shared_ptr<ASTNode> parseStmt();
  std::shared_ptr<ASTNode> parseBlockStmt();
  std::shared_ptr<ASTNode> parseDeclStmt();
  std::shared_ptr<ASTNode> parseExprStmt();
  std::shared_ptr<ASTNode> parseIfStmt();
  std::shared_ptr<ASTNode> parseForStmt();
  std::shared_ptr<ASTNode> parseBreakStmt();
  std::shared_ptr<ASTNode> parseContinueStmt();
  std::shared_ptr<ASTNode> parseExpr();
  std::shared_ptr<ASTNode> parseLogicExpr();
  std::shared_ptr<ASTNode> parseLogAndExpr();
  std::shared_ptr<ASTNode> parseBitOrExpr();
  std::shared_ptr<ASTNode> parseBitXorExpr();
  std::shared_ptr<ASTNode> parseBitAndExpr();
  std::shared_ptr<ASTNode> parseEqualExpr();
  std::shared_ptr<ASTNode> parseRelationExpr();
  std::shared_ptr<ASTNode> parseShiftExpr();
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