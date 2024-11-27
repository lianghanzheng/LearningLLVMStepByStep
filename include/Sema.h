#ifndef SEMA_H_
#define SEMA_H_

#include "Scope.h"
#include "AST.h"
#include "DiagEngine.h"

#include "llvm/ADT/StringRef.h"

#include <memory>

class Sema {
public:
  Sema(DiagEngine &diagEngine) : diagEngine(diagEngine) {}

  std::shared_ptr<ASTNode> 
  semaVariabelDeclNode(const Token &tok, CType *ty);

  std::shared_ptr<ASTNode> 
  semaVariableExprNode(const Token &tok);

  std::shared_ptr<ASTNode> semaAssignExprNode(
      std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaBinaryExprNode(
      OpCode op,
      std::shared_ptr<ASTNode> lhs, 
      std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaNumberExprNode(const Token &tok, CType *ty);

private:
  Scope scope;
  DiagEngine &diagEngine;
};

#endif