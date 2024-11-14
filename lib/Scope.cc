#include "Scope.h"
#include <memory>

Scope::Scope() {
  envs.push_back(std::make_shared<Env>());
}

void Scope::enterScope() {
  envs.push_back(std::make_shared<Env>());
}

void Scope::exitScop() {
  envs.pop_back();
}

std::shared_ptr<Symbol> 
Scope::findVarSymbol(llvm::StringRef name) {
  for (auto it = envs.rbegin(); it != envs.rend(); ++it) {
    auto &table = (*it)->symbolTable;
    if (table.count(name) > 0) {
      return table[name];   
    }
  }

  return nullptr;
}

std::shared_ptr<Symbol> 
Scope::findVarSymbolInCurEnv(llvm::StringRef name) {
  auto &table = envs.back()->symbolTable;
  if (table.count(name) > 0) {
    return table[name];   
  }

  return nullptr;
}

void Scope::addSymbol(SymbolKind kind, CType *ty, llvm::StringRef name) {
  auto symbol = std::make_shared<Symbol>(kind, ty, name);
  
  auto &table = envs.back()->symbolTable;
  table.insert({name, symbol});
}