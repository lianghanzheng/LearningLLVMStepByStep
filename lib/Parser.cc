#include "Parser.h"
#include "Lexer.h"
#include "Sema.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <cstdlib>
#include <memory>

std::shared_ptr<Program> Parser::parseProgram() {
  // Initialize member `tok` to the first token.
  advance();

  auto prog = std::make_shared<Program>();
  while (tok.tokenType != TokenType::eof) {
    if (tok.tokenType == TokenType::unknown) {
      llvm::errs() << "Unknown tokens\n";
      tok.dump();
      return nullptr;
    }
    
    if (tok.tokenType == TokenType::semi) {
      advance();
      continue;
    }

    if (tok.tokenType == TokenType::kw_int) {
      const auto &exprs = parseDecl();
      for (auto expr : exprs) {
        prog->exprVec.push_back(expr);
      }
    }
    else {
      auto expr = parseExpr();
      if (expr != nullptr) {
        prog->exprVec.push_back(expr);
      }
    }
  }

  return prog;
}

std::shared_ptr<ASTNode> Parser::parseExpr() {
  auto lhs = parseTerm();
  while (tok.tokenType == TokenType::plus || tok.tokenType == TokenType::minus) {
    OpCode op = tok.tokenType == TokenType::plus ? 
                OpCode::add : OpCode::sub;
    advance();
    
    auto rhs = parseTerm();
    auto binaryExpr = sema.semaBinaryExprNode(op, lhs, rhs);

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<ASTNode> Parser::parseTerm() {
  auto lhs = parseFactor();
  while (tok.tokenType == TokenType::star || tok.tokenType == TokenType::slash) {
    OpCode op = tok.tokenType == TokenType::star ? 
                OpCode::mul : OpCode::div;
    advance();
    
    auto rhs = parseFactor();
    auto binaryExpr = sema.semaBinaryExprNode(op, lhs, rhs); 

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<ASTNode> Parser::parseFactor() {
  if (tok.tokenType == TokenType::lparen) {
    advance();
    auto expr = parseExpr();
    assert(expect(TokenType::rparen));
    advance();
    return expr;
  }
  else if (tok.tokenType == TokenType::identifier) {
    auto expr = sema.semaVariableExprNode(tok.content);
    advance();
    return expr;
  }
  else {
    assert(expect(TokenType::number));
    auto factor = sema.semaNumberExprNode(tok.value, tok.kind);
    advance();
    return factor;
  }
}


std::vector<std::shared_ptr<ASTNode>> 
Parser::parseDecl() {
  consume(TokenType::kw_int);
  CType *baseType = CType::getIntTy();
  std::vector<std::shared_ptr<ASTNode>> astVec;

  int flag = 0; // Counter for ','
  while (tok.tokenType != TokenType::semi) {
    if (flag++ > 0) {
      assert(consume(TokenType::comma));      
    }
    
    llvm::StringRef varName = tok.content;
    auto varDecl = sema.semaVariabelDeclNode(varName, baseType);
    // int a = 1; <=> int a; a = 1;
    astVec.push_back(varDecl);

    consume(TokenType::identifier);

    if (tok.tokenType == TokenType::equal) {
      advance();
      auto rhs = parseExpr();
      auto varExpr = sema.semaVariableExprNode(varName);

      auto assignExpr = sema.semaAssignExprNode(varExpr, rhs);

      astVec.push_back(assignExpr);
    }
  }

  assert(consume(TokenType::semi) && 
                 "Expect `;` at the end of a statement\n");

  return astVec;
}

bool Parser::expect(TokenType tokenType) {
  return tok.tokenType == tokenType;
}

bool Parser::consume(TokenType tokenType) {
  if (expect(tokenType)) {
    advance();
    return true;
  }
  
  return false;
}

void Parser::advance() {
  lexer.nextToken(tok);
}