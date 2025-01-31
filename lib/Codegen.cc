#include "Codegen.h"
#include "AST.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
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

  currentFunction = mainFunc;

  llvm::BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entryBB);
  

  llvm::Value *finalValue = nullptr;
  for (auto &expr: prog->stmtVec) {
    llvm::Value *value = expr->accept(this);
    finalValue = value;
  }
  
  if (finalValue) {
    // To avoid the ConstantFolder in builder by default.
    (void)builder.CreateCall(printfFunc, {
        builder.CreateGlobalString("Expr value = %d\n"),
        finalValue
    });
  }
  else {
    llvm::outs() << "Last statement is not a expression statement\n";
  }
  
  (void)builder.CreateRet(builder.getInt32(0));

  verifyFunction(*mainFunc);
  m->print(llvm::outs(), nullptr);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitBlockStmt(BlockStmt *blockStmt) {
  llvm::Value *lastValue = nullptr;
  for (auto &stmt: blockStmt->stmtVec) {
    lastValue = stmt->accept(this);
  }

  return lastValue;
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
    builder.CreateCondBr(condVal, thenBB, lastBB);

    builder.SetInsertPoint(thenBB);
    ifStmt->thenBody->accept(this);
    builder.CreateBr(lastBB);
  }

  builder.SetInsertPoint(lastBB);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitForStmt(ForStmt *forStmt) {
  auto initBB = llvm::BasicBlock::Create(context, "for.init", currentFunction);
  auto condBB = llvm::BasicBlock::Create(context, "for.cond", currentFunction);
  auto incBB = llvm::BasicBlock::Create(context, "for.inc", currentFunction);
  auto bodyBB = llvm::BasicBlock::Create(context, "for.body", currentFunction);
  auto lastBB = llvm::BasicBlock::Create(context, "for.last", currentFunction);

  breakBBs.insert({forStmt, lastBB});
  continueBBs.insert({forStmt, incBB});

  builder.CreateBr(initBB);
  builder.SetInsertPoint(initBB);
  if (forStmt->initExpr) {
    forStmt->initExpr->accept(this);
  }
  builder.CreateBr(condBB);
  
  builder.SetInsertPoint(condBB);
  if (forStmt->condExpr) {
    llvm::Value *val = forStmt->condExpr->accept(this);
    llvm::Value *condVal = builder.CreateICmpNE(val, builder.getInt32(0));
    builder.CreateCondBr(condVal, bodyBB, lastBB);
  }
  else {
    builder.CreateBr(bodyBB);
  }

  builder.SetInsertPoint(bodyBB);
  if (forStmt->forBody) {
    forStmt->forBody->accept(this);
  }  
  builder.CreateBr(incBB);

  builder.SetInsertPoint(incBB);
  if (forStmt->incExpr) {
    forStmt->incExpr->accept(this);
  }
  builder.CreateBr(condBB);

  builder.SetInsertPoint(lastBB);

  breakBBs.erase(forStmt);
  continueBBs.erase(forStmt);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitBreakStmt(BreakStmt *breakStmt) {
  auto targetBB = breakBBs[breakStmt->target.get()];
  builder.CreateBr(targetBB);

  auto deathBB = llvm::BasicBlock::Create(context, "for.break.death", currentFunction);
  builder.SetInsertPoint(deathBB);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitContinueStmt(ContinueStmt *continueStmt) {
  auto targetBB = continueBBs[continueStmt->target.get()];
  builder.CreateBr(targetBB);

  auto deathBB = llvm::BasicBlock::Create(context, "for.continue.death", currentFunction);
  builder.SetInsertPoint(deathBB);

  return nullptr;
}

llvm::Value *CodegenVisitor::visitBinaryExpr(BinaryExpr *binaryExpr) {
  llvm::Value *value;

  switch (binaryExpr->op) {
  case OpCode::add: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateNSWAdd(lhs, rhs);
    return value;
  }
  case OpCode::sub: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateNSWSub(lhs, rhs);
    return value;
  }
  case OpCode::mul: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateNSWMul(lhs, rhs);
    return value;
  }
  case OpCode::div: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateSDiv(lhs, rhs);
    return value;
  }
  case OpCode::mod: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateSRem(lhs, rhs);
    return value;
  }
  case OpCode::logor: {
    llvm::BasicBlock *ltrue = llvm::BasicBlock::Create(context, "ltrue");
    llvm::BasicBlock *lfalse = llvm::BasicBlock::Create(context, "lfalse", currentFunction);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "mergeBB");

    auto lhs = binaryExpr->lhs->accept(this);
    llvm::Value *lhsCond = builder.CreateICmpNE(lhs, builder.getInt32(0));
    builder.CreateCondBr(lhsCond, ltrue, lfalse);

    builder.SetInsertPoint(lfalse);
    auto rhs = binaryExpr->rhs->accept(this);
    llvm::Value *rhsCond = builder.CreateICmpNE(rhs, builder.getInt32(0));
    rhsCond = builder.CreateZExt(rhsCond, builder.getInt32Ty());    
    builder.CreateBr(mergeBB);

    // Builder may create other basic blocks in the recursive decentdent.
    // The `lfalse` should may the basic block which create `rhsCond`.
    lfalse = builder.GetInsertBlock();

    ltrue->insertInto(currentFunction);
    builder.SetInsertPoint(ltrue);
    builder.CreateBr(mergeBB);    

    mergeBB->insertInto(currentFunction);
    builder.SetInsertPoint(mergeBB);
    llvm::PHINode *phi = builder.CreatePHI(builder.getInt32Ty(), 2);
    // Use the right relation.
    phi->addIncoming(rhsCond, lfalse);
    phi->addIncoming(builder.getInt32(1), ltrue);

    return phi;
  }
  case OpCode::logand: { 
    llvm::BasicBlock *ltrue = llvm::BasicBlock::Create(context, "ltrue", currentFunction);
    llvm::BasicBlock *lfalse = llvm::BasicBlock::Create(context, "lfalse", currentFunction);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "mergeBB", currentFunction);

    auto lhs = binaryExpr->lhs->accept(this);
    llvm::Value *lhsCond = builder.CreateICmpNE(lhs, builder.getInt32(0));
    builder.CreateCondBr(lhsCond, ltrue, lfalse);

    builder.SetInsertPoint(ltrue);
    auto rhs = binaryExpr->rhs->accept(this);
    llvm::Value *rhsCond = builder.CreateICmpNE(rhs, builder.getInt32(0));
    rhsCond = builder.CreateZExt(rhsCond, builder.getInt32Ty());    
    builder.CreateBr(mergeBB);

    ltrue = builder.GetInsertBlock();

    builder.SetInsertPoint(lfalse);
    builder.CreateBr(mergeBB);   

    builder.SetInsertPoint(mergeBB);
    llvm::PHINode *phi = builder.CreatePHI(builder.getInt32Ty(), 2);
    phi->addIncoming(rhsCond, ltrue);
    phi->addIncoming(builder.getInt32(0), lfalse);

    return phi;
  }
  case OpCode::bit_or: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateAnd(lhs, rhs);
    return value;
  }
  case OpCode::bit_xor: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateXor(lhs, rhs);
    return value;
  }
  case OpCode::bit_and: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateAnd(lhs, rhs);
    return value;
  }
  case OpCode::leftshift: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateShl(lhs, rhs);
    return value;
  }
  case OpCode::rightshift: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateAShr(lhs, rhs);
    return value;
  }
  case OpCode::equalequal: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpEQ(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  case OpCode::notequal: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpNE(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  case OpCode::less: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpSLT(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  case OpCode::lesseq: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpSLE(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  case OpCode::greater: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpSGT(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  case OpCode::greatereq: {
    auto lhs = binaryExpr->lhs->accept(this);
    auto rhs = binaryExpr->rhs->accept(this);
    value = builder.CreateICmpSGE(lhs, rhs);
    value = builder.CreateIntCast(value, builder.getInt32Ty(), true);
    return value;
  }
  }
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