# Exercise01: Hello World! - LLVM IR

* 创建 `LLVMContext`
* 创建 `Module`
* 创建全局变量
* 创建指针类型
* 函数声明和C语言函数库链接

生成代码如下：

```llvm
; ModuleID = 'helloworld'
source_filename = "helloworld"

@globalString = private constant [13 x i8] c"Hello world!\00"

declare i32 @puts(ptr)

define i32 @main() {
entry:
  %call_puts = call i32 @puts(ptr @globalString)
  ret i32 0
}
```

