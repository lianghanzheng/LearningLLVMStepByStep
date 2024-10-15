#ifndef EXEC2_AST_H_
#define EXEC2_AST_H_

#include <functional>
#include <memory>
#include <vector>

#include "llvm/IR/Value.h"

struct Program;
struct Expr;
struct BinaryExpr;
struct Factor;

struct Visitor {
  virtual ~Visitor() {}
  virtual llvm::Value *visitProgram(Program *) = 0;
  virtual llvm::Value *visitExpr(Expr *) { return nullptr; }
  virtual llvm::Value *visitBinaryExpr(BinaryExpr *) = 0;
  virtual llvm::Value *visitFactor(Factor *) = 0;
};

struct Expr {
  virtual ~Expr() {}
  virtual llvm::Value *accept(Visitor *visitor) { return nullptr; }
};

enum class OpCode {
  add, sub, mul, div
};

/// On behalf of `Expr` and `Term` within our grammer.
struct BinaryExpr : Expr {
  OpCode op;
  std::shared_ptr<Expr> lhs; 
  std::shared_ptr<Expr> rhs;

  llvm::Value *accept(Visitor *visitor) override {
    return visitor->visitBinaryExpr(this);
  }
};

struct Factor : Expr {
  int number;
  llvm::Value *accept(Visitor *visitor) override {
    return visitor->visitFactor(this);
  }
};

struct Program {
  std::vector<std::shared_ptr<Expr>> exprVec;
  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitProgram(this);
  }
};

#endif // EXEC2_AST_H_