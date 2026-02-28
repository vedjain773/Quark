# MINIC

Minic is a small compiler for a restricted subset of the C language, written in C++ and
targeting LLVM IR. The project is focused on understanding compiler architecture
end-to-end: lexing, parsing, semantic analysis, and code generation.

## Overview

This compiler implements a minimal but structured pipeline:
* Lexer – Tokenizes source code.
* Parser – Builds a strongly typed Abstract Syntax Tree (AST).
* Semantic Analysis – Performs scope resolution and type checking.
* Code Generation – Emits LLVM IR.

The goal of the project is correctness, clean architecture, and incremental feature
development rather than full C compliance.

## Currently Supported Language Features

### Types

* int
* char

### Functions

* Function definitions
* Function parameters
* Function calls
* Nested function calls
* Return statements with type validation

### Statements
* If-else blocks
* While loops

### Expression and variable semantics
* Integer literals
* Character literals
* Variables
* Local variable declarations
* Initialization at declaration
* Block-scoped variables
* Proper nested scope handling

### Binary arithmetic:
Supported operators:
* Addition(+)
* Subtraction(-)
* Multiplication(*)
* Integer division(/)
* Modulus(%)

### Semantic Guarantees
* All identifiers are resolved before code generation
* Distinction between functions and variables
* Function return types enforced
* Expression types computed and stored in AST

### Architecture Notes
* Strongly typed AST (no RTTI, no dynamic_cast)
* Uses std::unique_ptr for ownership
* Nested block structure represented explicitly
* Symbol tables handled during semantic analysis

## Example Supported Program
```
int add(int a, int b) {
  return a + b;
}

int main() {
  int x = 5;
  return add(x, 3);
}
```

Corresponding LLVM IR:
```
; ModuleID = 'Moddex'
source_filename = "Moddex"

define i32 @add(i32 %a, i32 %b) {
entry:
  %b2 = alloca i32, align 4
  %a1 = alloca i32, align 4
  store i32 %a, ptr %a1, align 4
  store i32 %b, ptr %b2, align 4
  %a3 = load i32, ptr %a1, align 4
  %b4 = load i32, ptr %b2, align 4
  %add = add nsw i32 %a3, %b4
  ret i32 %add
}

define i32 @main() {
entry:
  %x = alloca i32, align 4
  store i32 5, ptr %x, align 4
  %x1 = load i32, ptr %x, align 4
  %calltmp = call i32 @add(i32 %x1, i32 3)
  ret i32 %calltmp
}
```
