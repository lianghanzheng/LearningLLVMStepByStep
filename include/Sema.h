#ifndef SEMA_H_
#define SEMA_H_

#include "Scope.h"
#include "AST.h"
#include "DiagEngine.h"

#include "llvm/ADT/StringRef.h"

#include <memory>
#include <vector>

class Sema {
public:
  Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}

  std::shared_ptr<ASTNode>
  semaIfStmtNode(std::shared_ptr<ASTNode> codeExpr,
                 std::shared_ptr<ASTNode> thenBody,
                 std::shared_ptr<ASTNode> elseBody);

  std::shared_ptr<ASTNode> 
  semaVariableDeclNode(const Token &tok, CType *ty);

  std::shared_ptr<ASTNode> 
  semaVariableExprNode(const Token &tok);

  std::shared_ptr<ASTNode> semaAssignExprNode(
      std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaBinaryExprNode(
      OpCode op,
      std::shared_ptr<ASTNode> lhs, 
      std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaNumberExprNode(const Token &tok, CType *ty);

public:
  void enterScope() { scope.enterScope(); }
  void exitScope() { scope.enterScope(); }

private:
  Scope scope;
  DiagEngine &diagEngine;
};

#endif