#ifndef PRINTVISITOR_H_
#define PRINTVISITOR_H_

#include "AST.h"

struct PrintVisitor : Visitor {
  PrintVisitor(std::shared_ptr<Program> prog);

  llvm::Value *visitProgram(Program *) override;
  llvm::Value *visitBlockStmt(BlockStmt *) override;
  llvm::Value *visitDeclStmt(DeclStmt *) override;
  llvm::Value *visitIfStmt(IfStmt *) override;
  llvm::Value *visitForStmt(ForStmt *) override;
  llvm::Value *visitBreakStmt(BreakStmt *) override;
  llvm::Value *visitContinueStmt(ContinueStmt *) override;
  llvm::Value *visitVariableDecl(VariableDecl *) override;
  llvm::Value *visitAssignExpr(AssignExpr *) override;
  llvm::Value *visitBinaryExpr(BinaryExpr *) override;
  llvm::Value *visitNumberExpr(NumberExpr *) override;
  llvm::Value *visitVariableExpr(VariableExpr *) override;
};

#endif // PRINTVISITOR_H_