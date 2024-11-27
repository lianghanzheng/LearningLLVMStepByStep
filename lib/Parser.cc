#include "Parser.h"
#include "AST.h"
#include "DiagEngine.h"
#include "Lexer.h"
#include "Sema.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/raw_ostream.h"

#include <cassert>
#include <cstdlib>
#include <memory>

std::shared_ptr<Program> Parser::parseProgram() {
  // Initialize member `tok` to the first token.
  advance();

  auto prog = std::make_shared<Program>();
  while (tok.tokenType != TokenType::eof) {
    // Handle null_stmt.
    if (tok.tokenType == TokenType::semi) {
      advance();
      continue;
    }

    // Handle decl_stmt.
    if (tok.tokenType == TokenType::kw_int) {
      const auto &exprs = parseDeclStmt();
      for (auto expr : exprs) {
        prog->exprVec.push_back(expr);
      }
    }
    else { // handle expr_stmt
      auto expr = parseExprStmt();
      if (expr != nullptr) {
        prog->exprVec.push_back(expr);
      }
    }
  }

  return prog;
}

std::vector<std::shared_ptr<ASTNode>> 
Parser::parseDeclStmt() {
  consume(TokenType::kw_int);
  CType *baseType = CType::getIntTy();
  std::vector<std::shared_ptr<ASTNode>> astVec;

  int flag = 0; // Counter for ','
  while (tok.tokenType != TokenType::semi) {
    if (flag++ > 0) {
      consume(TokenType::comma);      
    }
    
    Token tmp = tok;
    auto varDecl = sema.semaVariabelDeclNode(tmp, baseType);
    // int a = 1; <=> int a; a = 1;
    astVec.push_back(varDecl);

    consume(TokenType::identifier);

    if (tok.tokenType == TokenType::equal) {
      advance();
      auto rhs = parseExpr();
      auto varExpr = sema.semaVariableExprNode(tmp);

      auto assignExpr = sema.semaAssignExprNode(varExpr, rhs);

      astVec.push_back(assignExpr);
    }
  }

  consume(TokenType::semi);

  return astVec;
}

std::shared_ptr<ASTNode> Parser::parseAssignExpr() {
  expect(TokenType::identifier); 
  auto lhsExpr = sema.semaVariableExprNode(tok);
  advance();
  consume(TokenType::equal); 
  auto rhs = parseAddsubExpr();

  return sema.semaAssignExprNode(lhsExpr, rhs);
}

std::shared_ptr<ASTNode> Parser::parseExprStmt() {
  auto expr = parseExpr();
  consume(TokenType::semi);

  return expr;
}

std::shared_ptr<ASTNode> Parser::parseExpr() {
  bool isAssignExpr = false;

  lexer.saveState();
  if (tok.tokenType == TokenType::identifier) {
    Token tmp;
    lexer.nextToken(tmp);
    if (tmp.tokenType == TokenType::equal) {
      isAssignExpr = true;
    }
  }
  lexer.restoreState();

  if (!isAssignExpr) {
    return parseAddsubExpr();
  }
  else {
    return parseAssignExpr();
  }

}

std::shared_ptr<ASTNode> Parser::parseAddsubExpr() {
  auto lhs = parseMuldivExpr();
  while (tok.tokenType == TokenType::plus || tok.tokenType == TokenType::minus) {
    OpCode op = tok.tokenType == TokenType::plus ? 
                OpCode::add : OpCode::sub;
    advance();
    
    auto rhs = parseMuldivExpr();
    auto binaryExpr = sema.semaBinaryExprNode(op, lhs, rhs);

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<ASTNode> Parser::parseMuldivExpr() {
  auto lhs = parsePrimaryExpr();
  while (tok.tokenType == TokenType::star || tok.tokenType == TokenType::slash) {
    OpCode op = tok.tokenType == TokenType::star ? 
                OpCode::mul : OpCode::div;
    advance();
    
    auto rhs = parsePrimaryExpr();
    auto binaryExpr = sema.semaBinaryExprNode(op, lhs, rhs); 

    lhs = binaryExpr;
  }

  return lhs;
}

std::shared_ptr<ASTNode> Parser::parsePrimaryExpr() {
  if (tok.tokenType == TokenType::lparen) {
    advance();
    auto expr = parseExpr();
    expect(TokenType::rparen);
    advance();
    return expr;
  }
  else if (tok.tokenType == TokenType::identifier) {
    auto expr = sema.semaVariableExprNode(tok);
    advance();
    return expr;
  }
  else {
    expect(TokenType::number);
    auto factor = sema.semaNumberExprNode(tok, tok.ty);
    advance();
    return factor;
  }
}

bool Parser::expect(TokenType tokenType) {
  if (tok.tokenType == tokenType) return true;

  getDiagEngine().report(
      llvm::SMLoc::getFromPointer(tok.content.begin()), 
      diag::err_expected_token, 
      Token::getSpellingText(tokenType),
      tok.content);

  return false;
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

DiagEngine &Parser::getDiagEngine() const {
  return lexer.getDiagEngine();
}