# Learning LLVM Step by Step: Notes

本仓库计划使用LLVM实现一个C99标准子集的C语言编译器。

其中 `exercise` 目录下包含了若干练手用的小程序，可以用来检验环境配置、检查回顾对于LLVM特定组件的使用方式。

## Prerequisite

本仓库代码在LLVM 20.0.0上进行了验证。

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release \
-DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_ENABLE_ASSERTIONS=OFF -DLLVM_OPTIMIZED_TABLEGEN=ON \
-DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;mlir;openmp" -DLLVM_BUILD_TOOLS=ON \
-DLLVM_ENABLE_EH=ON -DLLVM_ENABLE_RTTI=ON \
-DLLVM_PARALLEL_LINK_JOBS=16 -DCMAKE_INSTALL_PREFIX=/opt/llvm -DLLVM_ENABLE_DOXYGEN=OFF \
-DLLVM_ENABLE_SPHINX=OFF -DLLVM_ENABLE_LLD=OFF -DLLVM_ENABLE_BINDINGS=OFF -DLLVM_ENABLE_LIBXML2=OFF \
-DOPENMP_ENABLE_LIBOMPTARGET=OFF -DLLVM_STATIC_LINK_CXX_STDLIB=ON ../llvm
```

## 编译代码

```sh
cmake -B build -S .
cmake --build build
```

## 目前的进度

- [x] 非负整型及其四则运算
- [ ] 基本浮点数及其四则运算
- [x] 关系表达式
- [x] 逻辑表达式
- [x] 变量
- [x] `if` 语句
- [x] 嵌套语句
- [x] 循环
- [ ] 指针
- [ ] 数组
- [ ] 结构体
- [x] 注释
- [ ] 预处理器

## 一些随笔

1. 在Codegen中处理 `AssignExpr` 时直接返回store指令的 `Value` 会在LLVM内部产生一个死循环
2. 关系表达式会返回一个 `getInt1Ty` 的类型，直接使用这个value进行其它32位类型运算时会产生一个错误
3. 在生成 `break` 和 `continue` 代码时，需要插入新的基本块 (但是我在使用clang生成IR中没有看到因为这两条指令生成的新基本块)
4. 在使用 `llvm::codegen` 名称空间的一些函数时，如 `llvm::codegen::getMArch` 时，需要声明全局变量 `static llvm::codegen::RegisterCodeGenFlags CGF`，否则会引起段错误
5. 在生成逻辑表达式时，由于逻辑能够短路，所以要为每个条件表达式的操作数生成一个基本块，并用Phi节点传出结果

## 已知错误

1. 当最后一个字符出现错误时，如 `int a=1; a` 这样缺少分号时，诊断引擎会打印后续内容并标记字符的位置，而此时我们已经到了文件的末尾，导致段错误
2. `for (;;) ;` 会导致 `for.last` 没有前置的基本块而报错