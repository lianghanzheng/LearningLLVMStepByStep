#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "Codegen.h"
#include "Sema.h"
#include "DiagEngine.h"

#include "llvm/IR/Module.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"

#include <algorithm>
#include <memory>

int main(int argc, char **argv) {
  if (argc < 2) {
    llvm::errs() << "no input file\n";
    return 0;
  }

  const char *filename = argv[1];

  static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
      llvm::MemoryBuffer::getFile(filename);
  
  if (!buf) {
    llvm::errs() << "Failed to open the file\n";
    return 1;
  }

  //std::unique_ptr<llvm::MemoryBuffer>srcBuf = std::move(*buf);

  llvm::SourceMgr mgr;
  DiagEngine diagEngine(mgr);
  mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

  Lexer lexer(mgr, diagEngine);
  Token tok;
  //while (tok.tokenType != TokenType::eof) {
  //  lexer.nextToken(tok);
  //  tok.dump();
  //}
  
  Sema sema(diagEngine);
  Parser parser(lexer, sema);

  auto prog = parser.parseProgram();
  //PrintVisitor pv(prog);

  CodegenVisitor cg(prog);

  return 0;
}