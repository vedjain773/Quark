#ifndef STATEMENT_H
#define STATEMENT_H

#include "CodegenVis.hpp"
#include "Expression.hpp"
#include "ExternalDecl.hpp"
#include "Scope.hpp"
#include "Token.hpp"
#include "Visitor.hpp"
#include <vector>

class Statement {
  public:
    int line, column;
    virtual ~Statement() = default;
    virtual void accept(Visitor &visitor) = 0;
    virtual void codegen(CodegenVis &codegenvis) = 0;

    virtual bool isTerminator() { return false; }
};

class EmptyStmt : public Statement {
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class ExprStmt : public Statement {
  public:
    std::unique_ptr<Expression> expression;

    ExprStmt(std::unique_ptr<Expression> expr);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class BlockStmt : public Statement {
  public:
    std::vector<std::unique_ptr<Statement>> statements;

    void addStmt(std::unique_ptr<Statement> stmt);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class IfStmt : public Statement {
  public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    std::unique_ptr<Statement> elseStmt;

    IfStmt(std::unique_ptr<Expression> condition,
           std::unique_ptr<Statement> ifbody,
           std::unique_ptr<Statement> elsestmt);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class ElseStmt : public Statement {
  public:
    std::unique_ptr<Statement> body;

    ElseStmt(std::unique_ptr<Statement> elsebody);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class WhileStmt : public Statement {
  public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    WhileStmt(std::unique_ptr<Expression> condn,
              std::unique_ptr<Statement> whilebody);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class BreakStmt : public Statement {
  public:
    BreakStmt(int tline, int tcol);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
    bool isTerminator();
};

class ContinueStmt : public Statement {
  public:
    ContinueStmt(int tline, int tcol);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
    bool isTerminator();
};

class ReturnStmt : public Statement {
  public:
    std::unique_ptr<Expression> retExpr;

    ReturnStmt(std::unique_ptr<Expression> retexpr);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
    bool isTerminator();
};

class DeclStmt : public Statement {
  public:
    TypeKind *type;
    std::string name;
    std::unique_ptr<Expression> expression;

    DeclStmt(TypeKind *tk, std::string varname,
             std::unique_ptr<Expression> expr, int tline, int tcol);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

class StructField {
  public:
    TypeKind *type;
    std::string fName;
    int line, column;

    StructField(TypeKind *tk, std::string fieldName, int tline, int tcol);
    void accept(Visitor &visitor);
};

class StructDecl : public Statement, public ExternalDecl {
  public:
    std::string tag;
    std::vector<std::unique_ptr<StructField>> fields;

    StructDecl(std::string tag, int tline, int tcol);
    void addField(std::unique_ptr<StructField> field);
    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

#endif
