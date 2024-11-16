# Learning LLVM Step by Step: Notes

本仓库计划使用LLVM实现一个C99标准子集的C语言编译器。

其中 `exercise` 目录下包含了若干练手用的小程序，可以用来检验环境配置、检查回顾对于LLVM特定组件的使用方式。

## Prerequisite

本仓库代码在LLVM 20.0.0上进行了验证。

```sh
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_ASSERTIONS=OFF -DLLVM_OPTIMIZED_TABLEGEN=ON \
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

## 当前版本的缺陷

1. 在AST中的 `VariableExpr` 听起来像是 `a+b` 这样的表达式，其实它表示的是 `a` 这样的对标识符的访问

## 一些随笔

1. 在Codegen中处理 `AssignExpr` 时直接返回store指令的 `Value` 会在LLVM内部产生一个死循环