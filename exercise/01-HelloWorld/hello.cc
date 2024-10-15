#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Verifier.h"

using namespace llvm;

int main() {
  LLVMContext context;
  // ; ModuleID = 'helloworld'
  // source_filename = "helloworld"
  llvm::Module m("helloworld", context);
  llvm::IRBuilder<> builder(context); 

  // @globalString = private constant [14 x i8] c"Hello world!\00"
  llvm::Constant *c = ConstantDataArray::get(context, "Hello world!");
  llvm::GlobalVariable *globalVar = new GlobalVariable(
      m, c->getType(), true, 
      GlobalValue::LinkageTypes::PrivateLinkage, 
      c, "globalString");  

  // Create a declaration for C "int puts(char *)"
  //
  llvm::IntegerType *i32Type = IntegerType::getInt32Ty(context);
  llvm::IntegerType *i8Type = IntegerType::getInt8Ty(context);
  llvm::PointerType *i8Ptr = PointerType::get(i8Type, 0);
  llvm::FunctionType *putsType = FunctionType::get(
      i32Type, i8Ptr, false);
  llvm::Function *putsFunc = Function::Create(
      putsType, 
      GlobalValue::LinkageTypes::ExternalLinkage, 
      "puts", m);
  

  // declare i32 @main()
  // Or build the Type through IRBuilder:
  //   builder.getInt32Ty();
  llvm::FunctionType *funcType = FunctionType::get(i32Type, false);
  llvm::Function *mainFunc = Function::Create(
      funcType, 
      GlobalValue::LinkageTypes::ExternalLinkage,
      "main", m);
  
  // entry:
  llvm::BasicBlock *entryBB = BasicBlock::Create(context, "entry", mainFunc);
  builder.SetInsertPoint(entryBB); 

  llvm::Value *gepValue = builder.CreateGEP(
      globalVar->getType(), globalVar, 
      {builder.getInt64(0), builder.getInt64(0)});

  // %call_puts = call i32 @puts(ptr @globalString)
  builder.CreateCall(putsFunc, {gepValue}, "call_puts");

  // ret i32 0  
  builder.CreateRet(builder.getInt32(0));

  // Verify the FUnction and Module.
  llvm::verifyFunction(*mainFunc, &errs());
  llvm::verifyModule(m, &errs());

  m.print(outs(), nullptr);
  return 0;
}

/// 创建变量可以用IRBuilder，
/// 也可以用直接new一个出来。
/// 不过不知道是不是我的clangd配置的有问题，模板类的实例可能不会给成员函数的代码不全。


/// 这个程序中并没有创建getelementptr指令，是因为builder中folder的原因吗？