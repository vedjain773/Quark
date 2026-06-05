# Quark

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
* Lexer – Tokenizes source code
* Parser – Builds a strongly typed Abstract Syntax Tree (AST)
* Semantic Analysis – Performs scope resolution and type checking
* Code Generation – Emits LLVM IR
* Optimizer - Uses LLVM passes to apply optimizations on the IR

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

### Optimizations
The compiler performs the following optimizations throught LLVM passes:
* Power transforms
* Simple algebraic transformations
* Dead instruction elimination
* Mem2Reg (a hand written eqv of LLVM's mem2reg pass)
* Dead Branch elimination

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
```c
int main() {
    int i;
    i = 0;

    while (i < 5)
        i = i + 1;

    return i;
}
```

Corresponding LLVM IR (after applying optimizations) :
```llvm
; ModuleID = 'tests/while_1.c'
source_filename = "tests/while_1.c"

define i32 @main() {
entry:
  br label %cond

cond:                                             ; preds = %whilebody, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add, %whilebody ]
  %compSLT = icmp slt i32 %i.0, 5
  %ext = zext i1 %compSLT to i32
  %whilecond = icmp ne i32 %ext, 0
  br i1 %whilecond, label %whilebody, label %after

whilebody:                                        ; preds = %cond
  %add = add nsw i32 %i.0, 1
  br label %cond

after:                                            ; preds = %cond
  ret i32 %i.0
}
```
