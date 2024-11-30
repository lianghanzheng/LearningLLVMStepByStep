#include "Codegen.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Value.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include <asm-generic/errno.h>
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

  currentFunction = mainFunc;

  llvm::BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entryBB);
  

  llvm::Value *finalValue = nullptr;
  for (auto &expr: prog->stmtVec) {
    llvm::Value *value = expr->accept(this);
    finalValue = value;
  }
  
  // To avoid the ConstantFolder in builder by default.
  (void)builder.CreateCall(printfFunc, {
      builder.CreateGlobalString("Expr value = %d\n"),
      finalValue
  });
  
  (void)builder.CreateRet(builder.getInt32(0));

  verifyFunction(*mainFunc);
  m->print(llvm::outs(), nullptr);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitDeclStmt(DeclStmt *declStmt) {
  llvm::Value *lastValue;
  for (auto &expr: declStmt->exprVec) {
    lastValue = expr->accept(this);
  }

  return lastValue;
}

llvm::Value *CodegenVisitor::visitIfStmt(IfStmt *ifStmt) {
  llvm::BasicBlock *condBB = llvm::BasicBlock::Create(context, "if.cond", currentFunction);
  llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "if.then", currentFunction);
  llvm::BasicBlock *elseBB = nullptr;
  if (ifStmt->elseBody) {
     elseBB = llvm::BasicBlock::Create(context, "if.else", currentFunction);
  }
  llvm::BasicBlock *lastBB = llvm::BasicBlock::Create(context, "if.last", currentFunction);

  builder.CreateBr(condBB);

  builder.SetInsertPoint(condBB);
  llvm::Value *val = ifStmt->condExpr->accept(this);
  llvm::Value *condVal = builder.CreateICmpNE(val, builder.getInt32(0));
  
  if (ifStmt->elseBody) {
    builder.CreateCondBr(condVal, thenBB, elseBB);
    builder.SetInsertPoint(thenBB);
    ifStmt->thenBody->accept(this);
    builder.CreateBr(lastBB);

    builder.SetInsertPoint(elseBB);
    ifStmt->elseBody->accept(this);
    builder.CreateBr(lastBB); 
  }
  else {
    builder.CreateCondBr(condBB, thenBB, lastBB);

    builder.SetInsertPoint(thenBB);
    ifStmt->thenBody->accept(this);
    builder.CreateBr(lastBB);
  }

  builder.SetInsertPoint(lastBB);

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

llvm::Value *CodegenVisitor::visitVariableDecl(VariableDecl *variableDecl) {
  llvm::Type *ty = nullptr;
  if (variableDecl->ty == CType::getIntTy()) {
    ty = builder.getInt32Ty();
  }

  llvm::Value *declValue =  builder.CreateAlloca(ty, nullptr, variableDecl->tok.content);
  varAddrMap.insert({variableDecl->tok.content, declValue});

  return declValue;
}

llvm::Value *CodegenVisitor::visitAssignExpr(AssignExpr *assignExpr) {
  VariableExpr *varExpr = static_cast<VariableExpr *>(assignExpr->lhs.get());
  llvm::Value *lhsVar = varAddrMap[varExpr->name];
  llvm::Value *rhsValue =  assignExpr->rhs->accept(this);

  builder.CreateStore(rhsValue, lhsVar);
  
  // Fold the assign expression to get the address computation result.
  return rhsValue;
}

llvm::Value *CodegenVisitor::visitNumberExpr(NumberExpr *numberExpr) {
  return builder.getInt32(numberExpr->tok.value);
}

llvm::Value *CodegenVisitor::visitVariableExpr(VariableExpr *variableExpr) {
  llvm::Value *varAddr = varAddrMap[variableExpr->name];
  llvm::Type *ty = nullptr;
  if (variableExpr->ty == CType::getIntTy()) {
    ty = builder.getInt32Ty();
  }

  return builder.CreateLoad(ty, varAddr, variableExpr->name);
}