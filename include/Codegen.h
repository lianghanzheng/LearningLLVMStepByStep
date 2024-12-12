#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "AST.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

struct CodegenVisitor : Visitor {
  CodegenVisitor(std::shared_ptr<Program> prog);

  llvm::Value *visitProgram(Program *) override;
  llvm::Value *visitBlockStmt(BlockStmt *) override;
  llvm::Value *visitDeclStmt(DeclStmt *) override;
  llvm::Value *visitIfStmt(IfStmt *) override;
  llvm::Value *visitForStmt(ForStmt *) override;
  llvm::Value *visitBreakStmt(BreakStmt *) override;
  llvm::Value *visitContinueStmt(ContinueStmt *) override;
  llvm::Value *visitBinaryExpr(BinaryExpr *) override;
  llvm::Value *visitVariableDecl(VariableDecl *) override;
  llvm::Value *visitAssignExpr(AssignExpr *) override;
  llvm::Value *visitNumberExpr(NumberExpr *) override;
  llvm::Value *visitVariableExpr(VariableExpr *) override;

private:
  llvm::LLVMContext context;
  std::shared_ptr<llvm::Module> m;
  llvm::IRBuilder<> builder{context};

  llvm::StringMap<llvm::Value *> varAddrMap;
  llvm::DenseMap<ASTNode *, llvm::BasicBlock *> breakBBs;
  llvm::DenseMap<ASTNode *, llvm::BasicBlock *> continueBBs;

  
  llvm::Function *currentFunction;
};

#endif // CODEGEN_H_