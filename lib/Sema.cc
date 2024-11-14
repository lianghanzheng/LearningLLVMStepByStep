#include "Sema.h"
#include "AST.h"

#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>

std::shared_ptr<ASTNode> 
Sema::semaVariabelDeclNode(llvm::StringRef name, CType *ty) {
  std::shared_ptr<Symbol> symbol = scope.findVarSymbolInCurEnv(name);
  if (symbol) {
    llvm::errs() << "Redefinite variable " << name << "\n";
    return nullptr;
  }

  scope.addSymbol(SymbolKind::LocalVariable, ty, name);


  auto variableDecl = std::make_shared<VariableDecl>();
  variableDecl->name = name;
  variableDecl->ty = ty;
  
  return variableDecl;
}

std::shared_ptr<ASTNode> 
Sema::semaVariableExprNode(llvm::StringRef name) {
  std::shared_ptr<Symbol> symbol = scope.findVarSymbol(name);
  if (!symbol) {
    llvm::errs() << "Undefined variable " << name << "\n";
    return nullptr;
  }

  auto variableExpr = std::make_shared<VariableExpr>();
  variableExpr->name = name;
  variableExpr->ty = symbol->getTy();

  return variableExpr;
}

std::shared_ptr<ASTNode> Sema::semaAssignExprNode(
    std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs) {
  if (!lhs || !rhs) {
    llvm::errs() << "Left or right of assignment expression can't be resolved\n";
  }

  if (!llvm::isa<VariableExpr>(lhs.get())) {
    llvm::errs() << "Left of assignment expression should be a left value\n";
    return nullptr;
  }

  auto assignExpr = std::make_shared<AssignExpr>();
  assignExpr->lhs = lhs;
  assignExpr->rhs = rhs;

  return assignExpr;
}

std::shared_ptr<ASTNode> Sema::semaBinaryExprNode(
    OpCode op, 
    std::shared_ptr<ASTNode> lhs, 
    std::shared_ptr<ASTNode> rhs) {
  if (!lhs || !rhs) {
    llvm::errs() << "Left or right of binary expression can't be resolved\n";
    return nullptr;
  }

  auto binaryExpr = std::make_shared<BinaryExpr>();
  binaryExpr->op = op;
  binaryExpr->lhs = lhs;
  binaryExpr->rhs = rhs;

  return binaryExpr;
}

std::shared_ptr<ASTNode> Sema::semaNumberExprNode(int value, CType *ty) {
  auto numberExpr = std::make_shared<NumberExpr>();
  numberExpr->number = value;
  numberExpr->ty = ty;

  return numberExpr;
}