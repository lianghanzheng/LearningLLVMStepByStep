#include "Codegen.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <memory>

#define DEBUG_TYPE "CodeGen"

using namespace llvm;

CodegenVisitor::CodegenVisitor(std::shared_ptr<Program> program) {
  m = std::make_shared<llvm::Module>("exprmodule", context);
  visitProgram(program.get());
}

llvm::Value *CodegenVisitor::visitProgram(Program *prog) {
  llvm::PointerType *i8Ptr = PointerType::get(builder.getInt8Ty(), 0);
  llvm::FunctionType *printfType = FunctionType::get(
      builder.getInt32Ty(), i8Ptr, true);
  llvm::Function *printfFunc = Function::Create(
    printfType, GlobalVariable::ExternalLinkage, 
    "printf", m.get());

  llvm::FunctionType *mainType = FunctionType::get(builder.getInt32Ty(), false);
  llvm::Function *mainFunc = Function::Create(
      mainType, GlobalVariable::ExternalLinkage, 
      "main", m.get());

  llvm::BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entryBB);
  
  for(auto &expr: prog->exprVec) {
    llvm::Value *value = expr->accept(this);
    // To avoid the ConstantFolder in builder by default.
    (void)builder.CreateCall(printfFunc, {
        builder.CreateGlobalString("Expr value = %d\n"),
        value
    });
  }
  
  (void)builder.CreateRet(builder.getInt32(0));

  verifyFunction(*mainFunc);
  m->print(llvm::outs(), nullptr);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitBinaryExpr(BinaryExpr *binaryExpr) {
  auto lhs = binaryExpr->lhs->accept(this);
  auto rhs = binaryExpr->rhs->accept(this);
  llvm::Value *value;

  switch (binaryExpr->op) {
  case OpCode::add:
    value = builder.CreateNSWAdd(lhs, rhs);
    break;
  case OpCode::sub:
    value = builder.CreateNSWSub(lhs, rhs);
    break;
  case OpCode::mul:
    value = builder.CreateNSWMul(lhs, rhs);
    break;
  case OpCode::div:
    value = builder.CreateSDiv(lhs, rhs);
    break; 
  default:
    return nullptr;
  }

  return value;
}

llvm::Value *CodegenVisitor::visitVariableDecl(VariableDecl *) {
  return nullptr;
}

llvm::Value *CodegenVisitor::visitAssignExpr(AssignExpr *) {
  return nullptr;
}

llvm::Value *CodegenVisitor::visitNumberExpr(NumberExpr *numberExpr) {
  return builder.getInt32(numberExpr->number);
}

llvm::Value *CodegenVisitor::visitVariableExpr(VariableExpr *) {
  return nullptr;
}