![quark](assets/Quark.png)

Quark is a small compiler for a subset of the C language, written in C++ and
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
| --emit-llvm | Print LLVM IR to a file |
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
* uint8_t   [Built-in]
* uint16_t  [Built-in]
* Pointers
* User-defined types
    - Arrays (including multidimensional arrays)
    - Structs

### Functions

* Function definitions
* Function parameters
* Function calls
* Nested function calls
* Return statements with type validation

### Statements
* If-else blocks
* While loops
* Break statements
* Continue statements

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
* An abstract syntax tree (AST) printer

## Supported Operators

### Arithmetic Operators
| Operator  | Description   | Example |
|-----------|---------------|---------|
| +         | Addition      | a + b |
| -         | Subtraction   | a - b |
| *         | Multiplication| a * b |
| /         | Division      | a / b |
| %         | Modulus       | a % b |

### Comparison Operators
| Operator  | Description       | Example |
|-----------|-------------------|---------|
| ==        | Equal to          | a == b  |
| !=        | Not equal to      | a != b  |
| >         | Greater than      | a > b   |
| <         | Less than         | a < b   |
| >=        | Greater or equal  | a >= b  |
| <=        | Less or equal     | a <= b  |

### Logical Operators
| Operator | Description | Example |
|---------|-------------|---------|
| &&    | Logical AND   | a && b |
| \|\|  | Logical OR    | a \|\| b |
| !     | Logical NOT   | !a |

### Pointer Operators
| Operator | Description | Example |
|---------|-------------|---------|
| *     | Dereference           | *ptr |
| []    | Access array element  | arr[i] |
| &     | AddressOf             | &x |

### Member Access Operators
| Operator | Description | Example |
|---------|-------------|---------|
| .     | Dot   | [struct].member |
| ->    | Arrow | [struct*]->member |

### Assignment Operators
| Operator | Description | Example |
|---------|-------------|---------|
| = | Assignment | a = 5 |

### Miscellaneous Operators
| Operator | Description | Example |
|----------|-------------|---------|
| sizeof   | Size-of     | sizeof(i) or sizeof(int*)|

## Example Programs
Checkout the examples folder to test out some programs compiled by Quark

This also includes pixelc, which compiles code to WASM and uses it as an alternative to JS 
for image processing workflows (grayscaling and color inversion)

[pixelc](https://vedjain773.github.io/Quark/examples/pixelc/web/index.html)
