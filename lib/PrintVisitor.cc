#include "PrintVisitor.h"
#include "AST.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/PointerLikeTypeTraits.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdio>

PrintVisitor::PrintVisitor(std::shared_ptr<Program> prog) {
  visitProgram(prog.get());
}

llvm::Value *PrintVisitor::visitProgram(Program *prog) {
  for (const auto &expr: prog->stmtVec) {
    // 由基类进行类型分发
    expr->accept(this);
    llvm::outs() << "\n";
  }

  return nullptr;
}

llvm::Value *PrintVisitor::visitBlockStmt(BlockStmt *blockStmt) {
  llvm::outs() << "{ ";
  for (const auto &stmt: blockStmt->stmtVec) {
    stmt->accept(this);
    llvm::outs() << "; ";
  }
  llvm::outs() << "}";

  return nullptr;
}

llvm::Value *PrintVisitor::visitDeclStmt(DeclStmt *declStmt) {
  int elemIdx = 0;
  int size = declStmt->exprVec.size();
  for (const auto &expr: declStmt->exprVec) {
    expr->accept(this);
    elemIdx++;
    if (elemIdx < size)
    llvm::outs() << "; ";
  }

  return nullptr;
}

llvm::Value *PrintVisitor::visitIfStmt(IfStmt *ifStmt) {
  llvm::outs() << "if ";
  ifStmt->condExpr->accept(this);
  llvm::outs() << "\n  ";
  ifStmt->thenBody->accept(this); 
  llvm::outs() << "\n";
  
  if (ifStmt->elseBody) {
    llvm::outs() << "else\n  ";
    ifStmt->elseBody->accept(this);
  }
  
  return nullptr;
}

llvm::Value *PrintVisitor::visitForStmt(ForStmt *forStmt) {
  llvm::outs() << "for (";
  
  if (forStmt->initExpr) {
    forStmt->initExpr->accept(this);
  }
  llvm::outs() << ";";
  if (forStmt->condExpr) {
    forStmt->condExpr->accept(this);
  }
  llvm::outs() << ";";
  if (forStmt->incExpr) {
    forStmt->incExpr->accept(this);
  }
  llvm::outs() << ")\n";

  if (forStmt->forBody) {
    forStmt->forBody->accept(this);
  }
 
  return nullptr;
}

llvm::Value *PrintVisitor::visitBreakStmt(BreakStmt *breakStmt) {
  llvm::outs() << "break";
  return nullptr;
}

llvm::Value *PrintVisitor::visitContinueStmt(ContinueStmt *continueStmt) {
  llvm::outs() << "continue";
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
  case OpCode::mod:
    llvm::outs() << "%";
    break;
  case OpCode::equalequal:
    llvm::outs() << " == ";
    break;
  case OpCode::notequal:
    llvm::outs() << " != ";
    break;
  case OpCode::less:
    llvm::outs() << " < ";
    break;
  case OpCode::lesseq:
    llvm::outs() << " <= ";
    break;
  case OpCode::leftshift:
    llvm::outs() << "<<";
    break;
  case OpCode::greater:
    llvm::outs() << " > ";
    break;
  case OpCode::greatereq:
    llvm::outs() << " >= ";
    break;
  case OpCode::rightshift:
    llvm::outs() << ">>";
    break;
  case OpCode::logor:
    llvm::outs() << "||";
    break;
  case OpCode::logand:
    llvm::outs() << "&&";
    break;
  case OpCode::bit_or:
    llvm::outs() << "|";
    break;
  case OpCode::bit_xor:
    llvm::outs() << "^";
    break;
  case OpCode::bit_and:
    llvm::outs() << "&";
  }

  binaryExpr->rhs->accept(this);
  llvm::outs() << ")";

  return nullptr;
}

llvm::Value *PrintVisitor::visitNumberExpr(NumberExpr *numExpr) {
  llvm::outs() << numExpr->tok.value;

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