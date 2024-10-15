#include "Lexer.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ErrorOr.h"
#include <memory>

int main(int argc, char **argv) {
  if (argc < 2) {
    llvm::errs() << "no input file\n";
    return 0;
  }

  const char *filename = argv[1];
  
  static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =\
      llvm::MemoryBuffer::getFile(filename);
  
  if (!buf) {
    llvm::errs() << "Failed to open the file\n";
    return 1;
  }

  std::unique_ptr<llvm::MemoryBuffer>srcBuf = std::move(*buf);
  Lexer lex(srcBuf->getBuffer());
  
  Token tok;
  while (tok.tokenType != TokenType::eof) {
    lex.nextToken(tok);
    tok.dump();
  }
  return 0;
}