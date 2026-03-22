#ifndef OPTVIS_H
#define OPTVIS_H

#include <memory>

enum class Operators;

class Node;

class Expression;
class IntExpr;
class CharExpr;
class VarExpr;
class CallExpr;
class UnaryExpr;
class BinaryExpr;
class AssignExpr;
class EmptyExpr;

class Statement;
class ExprStmt;
class BlockStmt;
class IfStmt;
class ElseStmt;
class WhileStmt;
class ReturnStmt;
class DeclStmt;
class EmptyStmt;

class Parameter;
class Prototype;
class FuncDef;

class ExternalDecl;
class Program;

class OptimizeVisitor {
    public:
    int perfOp(int val1, int val2, Operators Op);
    char perfOp(char val1, char val2, Operators Op);

    int getIntVal(std::unique_ptr<Expression> intexpr);
    char getCharVal(std::unique_ptr<Expression> charexpr);

    bool isIdentityVal(Operators Op, bool left, int val);

    std::unique_ptr<Expression> visitExpr(IntExpr& intexpr);
    std::unique_ptr<Expression> visitExpr(CharExpr& charexpr);
    std::unique_ptr<Expression> visitExpr(VarExpr& varexpr);
    std::unique_ptr<Expression> visitExpr(CallExpr& callexpr);
    std::unique_ptr<Expression> visitExpr(UnaryExpr& unaryexpr);
    std::unique_ptr<Expression> visitExpr(BinaryExpr& binaryexpr);
    std::unique_ptr<Expression> visitExpr(AssignExpr& assignexpr);
    std::unique_ptr<Expression> visitExpr(EmptyExpr& emptyexpr);

    std::unique_ptr<Statement> visitStmt(ExprStmt& exprstmt);
    std::unique_ptr<Statement> visitStmt(BlockStmt& blockstmt);
    std::unique_ptr<Statement> visitStmt(IfStmt& ifstmt);
    std::unique_ptr<Statement> visitStmt(ElseStmt& elsestmt);
    std::unique_ptr<Statement> visitStmt(WhileStmt& whilestmt);
    std::unique_ptr<Statement> visitStmt(ReturnStmt& returnstmt);
    std::unique_ptr<Statement> visitStmt(DeclStmt& declstmt);
    std::unique_ptr<Statement> visitStmt(EmptyStmt& emptystmt);

    std::unique_ptr<Parameter> visitParameter(Parameter& parameter);
    std::unique_ptr<Prototype> visitPrototype(Prototype& prototype);
    std::unique_ptr<FuncDef> visitFuncDef(FuncDef& funcdef);

    std::unique_ptr<Program> visitProgram(Program& program);
};

#endif
