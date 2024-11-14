#ifndef EXEC2_PRINTVISITOR_H_
#define EXEC2_PRINTVISITOR_H_

#include "./AST.h"

struct PrintVisitor : Visitor {
  PrintVisitor(std::shared_ptr<Program> prog);

  llvm::Value *visitProgram(Program *) override;
  llvm::Value *visitBinaryExpr(BinaryExpr *) override;
  llvm::Value *visitFactor(Factor *) override;
};

#endif // EXEC2_PRINTVISITOR_H_