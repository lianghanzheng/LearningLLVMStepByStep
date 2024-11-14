#include "../include/Parser.h"
#include "../include/Lexer.h"
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

    auto expr = parseExpr();
    if (expr != nullptr) {
      prog->exprVec.push_back(expr);
    }
  }

  return prog;
}

std::shared_ptr<Expr> Parser::parseExpr() {
  auto lhs = parseTerm();
  while (tok.tokenType == TokenType::plus || tok.tokenType == TokenType::minus) {
    OpCode op = tok.tokenType == TokenType::plus ? 
                OpCode::add : OpCode::sub;
    advance();
    
    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->lhs = lhs;
    binaryExpr->rhs = parseTerm();

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<Expr> Parser::parseTerm() {
  auto lhs = parseFactor();
  while (tok.tokenType == TokenType::star || tok.tokenType == TokenType::slash) {
    OpCode op = tok.tokenType == TokenType::star ? 
                OpCode::mul : OpCode::div;
    advance();
    
    auto binaryExpr = std::make_shared<BinaryExpr>();
    binaryExpr->op = op;
    binaryExpr->lhs = lhs;
    binaryExpr->rhs = parseFactor();

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<Expr> Parser::parseFactor() {
  if (tok.tokenType == TokenType::lparen) {
    advance();
    auto expr = parseExpr();
    assert(expect(TokenType::rparen));
    advance();
    return expr;
  }
  else {
    assert(expect(TokenType::number));
    auto factor = std::make_shared<Factor>();
    factor->number = tok.value;
    advance();
    return factor;
  }
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