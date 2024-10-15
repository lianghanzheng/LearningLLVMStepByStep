# Exercise02: Non-variable Expression Compiler

计算一系列由 `;` 分隔的整数表达式。
因为没有变量，所以没有语义检查，整个编译过程就会非常简单。
语法定义见 [文档](./doc/Syntax.bnf)

* 使用LLVM设施读取文件
* 编写简单语法的解析器
* 基于Vistor模式的语法树打印和代码生成