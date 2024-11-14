#ifndef SEMA_H_
#define SEMA_H_

#include "Scope.h"
#include "AST.h"

#include "llvm/ADT/StringRef.h"

#include <memory>

class Sema {
public:
  std::shared_ptr<ASTNode> 
  semaVariabelDeclNode(llvm::StringRef name, CType *ty);

  std::shared_ptr<ASTNode> 
  semaVariableExprNode(llvm::StringRef name);

  std::shared_ptr<ASTNode> semaAssignExprNode(
      std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaBinaryExprNode(
      OpCode op,
      std::shared_ptr<ASTNode> lhs, 
      std::shared_ptr<ASTNode> rhs);

  std::shared_ptr<ASTNode> semaNumberExprNode(int value, CType *ty);

private:
  Scope scope;
};

#endif