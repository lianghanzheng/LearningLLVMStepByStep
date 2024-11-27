#include "PrintVisitor.h"
#include "AST.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"

PrintVisitor::PrintVisitor(std::shared_ptr<Program> prog) {
  visitProgram(prog.get());
}

llvm::Value *PrintVisitor::visitProgram(Program *prog) {
  for (auto &expr: prog->exprVec) {
    // 由基类进行类型分发
    expr->accept(this);
    llvm::outs() << ";\n";
  }

  return nullptr;
}

llvm::Value *PrintVisitor::visitBinaryExpr(BinaryExpr *binaryExpr) {

  llvm::outs() << "(";
  binaryExpr->lhs->accept(this);

  switch (binaryExpr->op) {
  case OpCode::add:
    llvm::outs() << " + ";
    break;
  case OpCode::sub:
    llvm::outs() << " - ";
    break;
  case OpCode::mul:
    llvm::outs() << " * ";
    break;
  case OpCode::div:
    llvm::outs() << " / ";
    break;
  }

  binaryExpr->rhs->accept(this);
  llvm::outs() << ")";

  return nullptr;
}

llvm::Value *PrintVisitor::visitNumberExpr(NumberExpr *numExpr) {
  llvm::outs() << numExpr->number;

  return nullptr;
}


llvm::Value *PrintVisitor::visitVariableDecl(VariableDecl *variableDecl) {
  if (variableDecl->ty == CType::getIntTy()) {
    llvm::outs() << "int " << variableDecl->tok.content;
  }

  return nullptr;
}


llvm::Value *PrintVisitor::visitAssignExpr(AssignExpr *assignExpr) {
  llvm::outs() << "(";
  assignExpr->lhs->accept(this);
  llvm::outs() << " = ";
  assignExpr->rhs->accept(this);
  llvm::outs() << ")";

  return nullptr;
}

llvm::Value *PrintVisitor::visitVariableExpr(VariableExpr *variableExpr) {
  llvm::outs() << variableExpr->name;
  return nullptr;
}