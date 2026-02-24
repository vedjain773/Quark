#ifndef STATEMENT_H
#define STATEMENT_H

#include <vector>
#include "Expression.hpp"
#include "Token.hpp"
#include "Visitor.hpp"
#include "CodegenVis.hpp"

class Statement {
    public:
    int line;
    int column;
    virtual ~Statement() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual void codegen(CodegenVis& codegenvis) = 0;
};

class EmptyStmt: public Statement {
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class ExprStmt: public Statement {
    public:
    std::unique_ptr<Expression> expression;

    ExprStmt(std::unique_ptr<Expression> expr);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class BlockStmt: public Statement {
    public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStmt(std::unique_ptr<Statement> stmt);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class IfStmt: public Statement {
    public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    std::unique_ptr<Statement> elseStmt;

    IfStmt(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> ifbody, std::unique_ptr<Statement> elsestmt);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class ElseStmt: public Statement {
    public:
    std::unique_ptr<Statement> body;

    ElseStmt(std::unique_ptr<Statement> elsebody);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class WhileStmt: public Statement {
    public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStmt(std::unique_ptr<Expression> condn, std::unique_ptr<Statement> whilebody);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class ReturnStmt: public Statement {
    public:
    std::unique_ptr<Expression> retExpr;

    ReturnStmt(std::unique_ptr<Expression> retexpr);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

class DeclStmt: public Statement {
    public:
    TokenType type;
    std::string name;
    std::unique_ptr<Expression> expression;

    DeclStmt(TokenType tokentype, std::string varname, std::unique_ptr<Expression> expr, int tline, int tcol);
    void accept(Visitor& visitor);
    void codegen(CodegenVis& codegenvis);
};

#endif
