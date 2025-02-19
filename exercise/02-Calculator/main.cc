#include "./include/Lexer.h"
#include "./include/Parser.h"
#include "./include/PrintVisitor.h"
#include "./include/Codegen.h"
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
  
  static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
      llvm::MemoryBuffer::getFile(filename);
  
  if (!buf) {
    llvm::errs() << "Failed to open the file\n";
    return 1;
  }

  std::unique_ptr<llvm::MemoryBuffer>srcBuf = std::move(*buf);
  Lexer lexer(srcBuf->getBuffer());
  Parser parser(lexer);

  auto prog = parser.parseProgram();
  //PrintVisitor pv(prog);

  CodegenVisitor cg(prog);

  return 0;
}