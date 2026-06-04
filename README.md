# MINIC

Quark is a small compiler for a restricted subset of the C language, written in C++ and
targeting LLVM IR.

## Usage
Clone the repository
```bash
git clone https://github.com/vedjain773/quark.git && cd quark 
```

Build the project
```bash
#To build with g++
make

#To build with clang++
make clang
```

Compile source
```bash
./quark input.c -o output.o
```

Link with an existing C/C++ file
```bash
g++ link.cpp output.o -o output
```

Execute
```bash
./output
```
| Flag      | Description           |
|-----------|-----------------------|
| --pt      | Print tokens          |
| --past    | Print AST             |
| --emit-ir | Print LLVM IR         |
| -o        | Emit Object file      |
| -z        | Apply Optimizations   |

## Overview

This compiler implements a minimal but structured pipeline:
* Lexer – Tokenizes source code.
* Parser – Builds a strongly typed Abstract Syntax Tree (AST).
* Semantic Analysis – Performs scope resolution and type checking.
* Code Generation – Emits LLVM IR.

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

### Misc
* Pointers and pointer arithmetic
* An abstract syntax tree (AST) printer

## Supported Operators

### Arithmetic Operators
| Operator | Description | Example |
|---------|-------------|---------|
| + | Addition | a + b |
| - | Subtraction | a - b |
| * | Multiplication | a * b |
| / | Division | a / b |
| % | Modulus | a % b |

### Comparison Operators
| Operator | Description | Example |
|---------|-------------|---------|
| == | Equal to | a == b |
| != | Not equal to | a != b |
| > | Greater than | a > b |
| < | Less than | a < b |
| >= | Greater or equal | a >= b |
| <= | Less or equal | a <= b |

### Logical Operators
| Operator | Description | Example |
|---------|-------------|---------|
| && | Logical AND | a && b |
| \|\| | Logical OR | a \|\| b |
| ! | Logical NOT | !a |

### Pointer Operators
| Operator | Description | Example |
|---------|-------------|---------|
| * | Dereference   | *ptr  |
| & | AddressOf     | &x    |

### Assignment Operators
| Operator | Description | Example |
|---------|-------------|---------|
| = | Assignment | a = 5 |

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
; ModuleID = 'test/sample.c'
source_filename = "test/sample.c"

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
