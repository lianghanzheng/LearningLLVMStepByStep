#ifndef SCOPE_H_
#define SCOPE_H_

#include "Type.h"

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <vector>

enum class SymbolKind {
  LocalVariable,  
};

class Symbol {
public:
  Symbol(SymbolKind kind, CType *ty, llvm::StringRef name)
      : kind(kind), ty(ty), name(name) {}

  CType *getTy() const { return ty; }

private:
  SymbolKind kind;
  CType *ty;
  llvm::StringRef name;  
};

struct Env {
public:
  llvm::StringMap<std::shared_ptr<Symbol>> symbolTable;
};

class Scope {
public:
  Scope();
  void enterScope();
  void exitScope();
  std::shared_ptr<Symbol> findVarSymbol(llvm::StringRef name);
  std::shared_ptr<Symbol> findVarSymbolInCurEnv(llvm::StringRef name);
  void addSymbol(SymbolKind kind, CType *ty, llvm::StringRef name);

private:
  std::vector<std::shared_ptr<Env>> envs;

};

#endif // SCOPE_H_