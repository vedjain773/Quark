#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>
#include <memory>
#include "Scope.hpp"
#include "Token.hpp"
#include "Visitor.hpp"
#include "CodegenVis.hpp"

enum class Operators {
    //Unary
    BANG, MINUS,

    //Binary
    MODULUS, DIVIDE, MULT, PLUS,

    //Comparison
    GREATER, GREATER_EQUALS, LESS, LESS_EQUALS,

    //Equality
    EQUALS, NOT_EQUALS,

    //Logical
    AND, OR,
};

std::string getOpStr(Operators op);
Operators getOp(std::string op_str);

class Expression {
    public:
    TypeKind infType;
    int line;
    int column;
    virtual ~Expression() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual llvm::Value* codegen(CodegenVis& codegenvis) = 0;
};

class IntExpr: public Expression {
    public:
    int Val;

    IntExpr(int value, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class CharExpr: public Expression {
    public:
    char character;

    CharExpr(char charac, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class VarExpr: public Expression {
    public:
    std::string Name;

    VarExpr(std::string name, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class UnaryExpr: public Expression {
    public:
    Operators Op;
    std::unique_ptr<Expression> Operand;

    UnaryExpr(Operators op, std::unique_ptr<Expression> operand, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class BinaryExpr: public Expression {
    public:
    Operators Op;
    std::unique_ptr<Expression> LHS;
    std::unique_ptr<Expression> RHS;

    BinaryExpr(Operators op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class AssignExpr: public Expression {
    public:
    std::unique_ptr<Expression> LHS;
    std::unique_ptr<Expression> RHS;

    AssignExpr(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, int tline, int tcol);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class EmptyExpr: public Expression {
    public:
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

class CallExpr: public Expression {
    public:
    std::string callee;
    std::vector<std::unique_ptr<Expression>> args;

    CallExpr(std::string callee_name, int tline, int tcol);
    void add(std::unique_ptr<Expression> arg);
    void accept(Visitor& visitor);
    llvm::Value* codegen(CodegenVis& codegenvis);
};

#endif
