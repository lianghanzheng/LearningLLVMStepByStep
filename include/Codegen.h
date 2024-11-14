#ifndef CODEGEN_H_
#define CODEGEN_H_

#include "AST.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

struct CodegenVisitor : Visitor {
  CodegenVisitor(std::shared_ptr<Program> prog);

  llvm::Value *visitProgram(Program *) override;
  llvm::Value *visitBinaryExpr(BinaryExpr *) override;
  llvm::Value *visitVariableDecl(VariableDecl *) override;
  llvm::Value *visitAssignExpr(AssignExpr *) override;
  llvm::Value *visitNumberExpr(NumberExpr *) override;
  llvm::Value *visitVariableExpr(VariableExpr *) override;

private:
  llvm::LLVMContext context;
  std::shared_ptr<llvm::Module> m;
  llvm::IRBuilder<> builder{context};
};

#endif // CODEGEN_H_