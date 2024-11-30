#include "Sema.h"
#include "AST.h"
#include "DiagEngine.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"

#include <memory>


std::shared_ptr<ASTNode>
Sema::semaIfStmtNode(
    std::shared_ptr<ASTNode> condExpr, 
    std::shared_ptr<ASTNode> thenBody, 
    std::shared_ptr<ASTNode> elseBody) {
  assert((condExpr && thenBody) && 
         "The condition expression or then body of if statement is NULL\n");
  
  auto ifStmt = std::make_shared<IfStmt>();
  ifStmt->condExpr = condExpr;
  ifStmt->thenBody = thenBody;
  ifStmt->elseBody = elseBody;

  return ifStmt;
}

std::shared_ptr<ASTNode> 
Sema::semaVariableDeclNode(const Token &tok, CType *ty) {
  llvm::StringRef name = tok.content;
  std::shared_ptr<Symbol> symbol = scope.findVarSymbolInCurEnv(name);
  
  if (symbol) {
    diagEngine.report(
      llvm::SMLoc::getFromPointer(tok.content.begin()),
      diag::err_redefined,
      tok.content);
  }

  scope.addSymbol(SymbolKind::LocalVariable, ty, name);


  auto variableDecl = std::make_shared<VariableDecl>();
  variableDecl->tok.content = name;
  variableDecl->ty = ty;
  
  return variableDecl;
}

std::shared_ptr<ASTNode> 
Sema::semaVariableExprNode(const Token &tok) {
  llvm::StringRef name = tok.content;
  std::shared_ptr<Symbol> symbol = scope.findVarSymbol(name);
  if (!symbol) {
    diagEngine.report(
      llvm::SMLoc::getFromPointer(tok.content.begin()),
      diag::err_undefined,
      tok.content);
  }

  auto variableExpr = std::make_shared<VariableExpr>();
  variableExpr->name = name;
  variableExpr->ty = symbol->getTy();

  return variableExpr;
}

std::shared_ptr<ASTNode> Sema::semaAssignExprNode(
    std::shared_ptr<ASTNode> lhs, std::shared_ptr<ASTNode> rhs) {
  assert((lhs && rhs) && 
         "Left or right of assignment expression can't be resolved\n");

  if (!llvm::isa<VariableExpr>(lhs.get())) {
    diagEngine.report(
        llvm::SMLoc::getFromPointer(lhs->tok.content.begin()),
        diag::err_lvalue);
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
  assert((lhs && rhs) && 
         "Left or right of assignment expression can't be resolved\n");

  auto binaryExpr = std::make_shared<BinaryExpr>();
  binaryExpr->op = op;
  binaryExpr->lhs = lhs;
  binaryExpr->rhs = rhs;

  return binaryExpr;
}

std::shared_ptr<ASTNode> Sema::semaNumberExprNode(const Token &tok, CType *ty) {
  auto numberExpr = std::make_shared<NumberExpr>();
  numberExpr->tok = tok;
  numberExpr->number = tok.value;
  numberExpr->ty = ty;

  return numberExpr;
}