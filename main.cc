#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "PrintVisitor.h"
#include "Codegen.h"
#include "Sema.h"
#include "DiagEngine.h"
#include "Basic.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/CodeGen/CommandFlags.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassManagers.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/SMLoc.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/ToolOutputFile.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/WithColor.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/TargetParser/Triple.h"
#include "llvm/TargetParser/Host.h"


#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <string>
#include <system_error>

static llvm::codegen::RegisterCodeGenFlags CGF;

static llvm::cl::opt<std::string>
    InputFile(llvm::cl::Positional, 
              llvm::cl::desc("<input-files>"),
              llvm::cl::init("-"));

static llvm::cl::opt<std::string>
    OutputFile("o",
               llvm::cl::desc("<output-file>"),
               llvm::cl::value_desc("filename"));

static llvm::cl::opt<std::string> MTriple(
    "mtriple",
    llvm::cl::desc("Override target triple for module"));

static llvm::cl::opt<bool> EmitLLVM(
    "emit-llvm",
    llvm::cl::desc("Emit IR code instread of assembler"),
    llvm::cl::init(false));

static const char* Head = "tinycc - A simple C compiler";

void printVersion(llvm::raw_ostream &OS) {
  OS << Head << " " << getTinyccVeriosn() << "\n";
  OS << "  Default target: "
     << llvm::sys::getDefaultTargetTriple() << "\n";
  
  std::string CPU(llvm::sys::getHostCPUName());
  OS << "  Host CPU: " << CPU << "\n\n";
  OS.flush();
  llvm::TargetRegistry::printRegisteredTargetsForVersion(OS);
  exit(EXIT_SUCCESS);
}

llvm::TargetMachine *
createTargetMachine(const char *Argv0) {
  using namespace llvm;
  
  llvm::Triple Triple = llvm::Triple(
      !MTriple.empty()
          ? llvm::Triple::normalize(MTriple)
          : llvm::sys::getDefaultTargetTriple());

  llvm::TargetOptions TargetOptions =
      codegen::InitTargetOptionsFromCodeGenFlags(Triple);
  std::string CPUStr = codegen::getCPUStr();
  std::string FeatureStr = codegen::getFeaturesStr();

  std::string Error;
  const llvm::Target *Target =
      llvm::TargetRegistry::lookupTarget(
          codegen::getMArch(), Triple, Error);

  if (!Target) {
    llvm::WithColor::error(llvm::errs(), Argv0) << Error;
    return nullptr;
  }

  llvm::TargetMachine *TM = Target->createTargetMachine(
      Triple.getTriple(), CPUStr, FeatureStr, TargetOptions,
      std::optional<llvm::Reloc::Model>(
          codegen::getRelocModel()));
  return TM;
}

bool emit(llvm::StringRef Argv0, llvm::Module *M,
          llvm::TargetMachine *TM,
          llvm::StringRef InputFileName) {
  llvm::CodeGenFileType FT = llvm::codegen::getFileType();
  
  if (OutputFile.empty()) {
    if (InputFileName == "-") {
      OutputFile = "-";
    }
    else {
      if (InputFileName.ends_with(".c")) {
        OutputFile = InputFileName.drop_back(2).str();
      } 
      else if (InputFileName.ends_with(".txt")) {
        OutputFile = InputFileName.drop_back(4).str();
      }
      else {
        OutputFile = InputFileName.str();
      }  

      switch (FT) {
      case llvm::CGFT_AssemblyFile:
        OutputFile.append(EmitLLVM ? ".ll" : ".s");
        break;
      case llvm::CGFT_ObjectFile:
        OutputFile.append(".o");
        break;
      case llvm::CGFT_Null:
        OutputFile.append("null");
      }
    }
  }

  std::error_code EC;
  llvm::sys::fs::OpenFlags OF = llvm::sys::fs::OF_None;
  if (FT == llvm::CGFT_AssemblyFile) {
    OF |= llvm::sys::fs::OF_TextWithCRLF;
  }
  auto Out = std::make_unique<llvm::ToolOutputFile>(
    OutputFile, EC, OF);
  
  if (EC) {
    llvm::WithColor::error(llvm::errs(), Argv0)
        << EC.message() << "\n";
  
    return false;
  }
  
  llvm::legacy::PassManager PM;
  if (FT == llvm::CGFT_AssemblyFile && EmitLLVM) {
    PM.add(llvm::createPrintModulePass(Out->os()));
  }
  else { // TODO: Only support the generation for .ll file now.
    if (TM->addPassesToEmitFile(PM, Out->os(), nullptr, FT)) {
      llvm::WithColor::error(llvm::errs(), Argv0)
          << "No support for file type\n";
      return false;
    }
  }

  PM.run(*M);
  Out->keep();
  return true;
}

int main(int argc, const char **argv) {
  llvm::InitLLVM X(argc, argv);

  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  llvm::cl::SetVersionPrinter(&printVersion);
  llvm::cl::ParseCommandLineOptions(argc, argv, Head);

  llvm::TargetMachine *TM = createTargetMachine(argv[0]);
  if (!TM) exit(EXIT_FAILURE);

  static llvm::ErrorOr<std::unique_ptr<llvm::MemoryBuffer>> buf =
      llvm::MemoryBuffer::getFile(InputFile);
  
  if (!buf) {
    llvm::WithColor::error(llvm::errs(), argv[0])
        << "Failed to open the file " << InputFile << ": "
        << buf.getError().message() << "\n";
    exit(EXIT_FAILURE);
  }

  llvm::SourceMgr mgr;
  DiagEngine diagEngine(mgr);
  mgr.AddNewSourceBuffer(std::move(*buf), llvm::SMLoc());

  Lexer lexer(mgr, diagEngine);
  Token tok;
  //while (true) {
  //  lexer.nextToken(tok);
  //  if (tok.tokenType == TokenType::eof) {
  //    break;
  //  }
  //  tok.dump();
  //}
  
  Sema sema(diagEngine);
  Parser parser(lexer, sema);

  auto prog = parser.parseProgram();
  //PrintVisitor pv(prog);

  CodegenVisitor cg(prog);

  llvm::Module *M = cg.getModule();
  if (!emit(argv[0], M, TM, InputFile)) {
    llvm::WithColor::error(llvm::errs(), argv[0])
      << "Error writing output\n";
  }

  return 0;
}