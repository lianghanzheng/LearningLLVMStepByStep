#ifndef AST_H_
#define AST_H_

#include "Type.h"
#include "Lexer.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"

#include <functional>
#include <memory>
#include <vector>

struct Program;
struct BlockStmt;
struct DeclStmt;
struct IfStmt;
struct ASTNode;
struct VariableDecl;
struct AssignExpr;
struct BinaryExpr;
struct NumberExpr;
struct VariableExpr;

struct Visitor {
  virtual ~Visitor() {}
  virtual llvm::Value *visitProgram(Program *) = 0;
  virtual llvm::Value *visitBlockStmt(BlockStmt *) = 0;
  virtual llvm::Value *visitDeclStmt(DeclStmt *) = 0;
  virtual llvm::Value *visitIfStmt(IfStmt *) = 0;
  virtual llvm::Value *visitASTNode(ASTNode *) { return nullptr; }
  virtual llvm::Value *visitVariableDecl(VariableDecl *) = 0;
  virtual llvm::Value *visitAssignExpr(AssignExpr *) = 0;
  virtual llvm::Value *visitBinaryExpr(BinaryExpr *) = 0;
  virtual llvm::Value *visitNumberExpr(NumberExpr *) = 0;
  virtual llvm::Value *visitVariableExpr(VariableExpr *) = 0;
};

struct ASTNode {
  virtual ~ASTNode() {}
  virtual llvm::Value *accept(Visitor *visitor) { return nullptr; }
  CType *ty;
  Token tok;

  enum NodeKind {
    BlockStmt,
    DeclStmt,
    IfStmt,
    VariableDecl,
    BinaryExpr,
    NumberExpr,
    VariableExpr,
    AssignExpr
  };

  ASTNode(NodeKind kind) : kind(kind) {}
  const NodeKind getNodeKind() const { return kind; }

  NodeKind kind;
};

// TODO: Should we declare statements in a seperate file?
struct BlockStmt : ASTNode {
  BlockStmt() : ASTNode(NodeKind::BlockStmt) {}

  // TODO: We have not abstract Stmts as an independent Base Class.
  std::vector<std::shared_ptr<ASTNode>> stmtVec;

  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitBlockStmt(this);
  }

  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::BlockStmt;
  }
};

struct DeclStmt : ASTNode {
  DeclStmt() : ASTNode(NodeKind::DeclStmt) {}

  std::vector<std::shared_ptr<ASTNode>> exprVec;

  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitDeclStmt(this);
  } 

  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::DeclStmt;
  }
};

struct IfStmt : ASTNode {
  IfStmt() : ASTNode(NodeKind::IfStmt) {}
  
  std::shared_ptr<ASTNode> condExpr;
  std::shared_ptr<ASTNode> thenBody;
  std::shared_ptr<ASTNode> elseBody;

  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitIfStmt(this);
  }

  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::IfStmt;
  }
};

struct VariableDecl : ASTNode {
  VariableDecl() : ASTNode(NodeKind::VariableDecl) {}

  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitVariableDecl(this);
  }
  
  // Support rtti feature like llvm::isa.
  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::VariableDecl;
  }
};

struct AssignExpr : ASTNode {
  AssignExpr() : ASTNode(NodeKind::AssignExpr) {}

  std::shared_ptr<ASTNode> lhs; 
  std::shared_ptr<ASTNode> rhs;

  llvm::Value* accept(Visitor *visitor) {
    return visitor->visitAssignExpr(this);
  }

  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::AssignExpr;
  }
};

enum class OpCode {
  add, sub, mul, div
};

/// On behalf of `Expr` and `Term` within our grammer.
struct BinaryExpr : ASTNode {
  BinaryExpr() : ASTNode(NodeKind::BinaryExpr) {}

  OpCode op;
  std::shared_ptr<ASTNode> lhs; 
  std::shared_ptr<ASTNode> rhs;

  llvm::Value *accept(Visitor *visitor) override {
    return visitor->visitBinaryExpr(this);
  }

  // Available cast even if rtti is not enabled.
  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::BinaryExpr;
  }
};

struct NumberExpr : ASTNode {
  NumberExpr() : ASTNode(NodeKind::NumberExpr) {}

  int number;
  llvm::Value *accept(Visitor *visitor) override {
    return visitor->visitNumberExpr(this);
  }

  // Available cast even if rtti is not enabled.
  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::NumberExpr;
  }
};

struct VariableExpr : ASTNode {
  VariableExpr() : ASTNode(NodeKind::VariableExpr) {}

  llvm::StringRef name;
  llvm::Value *accept(Visitor *visitor) override {
    return visitor->visitVariableExpr(this);
  }

  // Available cast even if rtti is not enabled.
  static bool classof(const ASTNode *node) {
    return node->getNodeKind() == NodeKind::VariableExpr;
  }
};

struct Program {
  std::vector<std::shared_ptr<ASTNode>> stmtVec;
  llvm::Value *accept(Visitor *visitor) {
    return visitor->visitProgram(this);
  }
};

#endif // AST_H_