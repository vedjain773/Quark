#ifndef VISITOR_H
#define VISITOR_H

#include "Scope.hpp"
#include <string>
#include <vector>

class IntExpr;
class CharExpr;
class VarExpr;
class CallExpr;
class CastExpr;
class DerefExpr;
class AddressExpr;
class UnaryExpr;
class BinaryExpr;
class AssignExpr;
class EmptyExpr;

class ExprStmt;
class BlockStmt;
class IfStmt;
class ElseStmt;
class WhileStmt;
class ReturnStmt;
class DeclStmt;
class EmptyStmt;

class StructDecl;
class StructField;

class Parameter;
class Prototype;
class FuncDef;

class ExternalDecl;
class Program;

class Visitor {
  public:
    int depth = 0;

    virtual void visitIntExpr(IntExpr &intexpr) = 0;
    virtual void visitCharExpr(CharExpr &charexpr) = 0;
    virtual void visitVarExpr(VarExpr &varexpr) = 0;
    virtual void visitCallExpr(CallExpr &callexpr) = 0;
    virtual void visitCastExpr(CastExpr &castexpr) = 0;
    virtual void visitDerefExpr(DerefExpr &derefexpr) = 0;
    virtual void visitAddressExpr(AddressExpr &addressexpr) = 0;
    virtual void visitUnaryExpr(UnaryExpr &unaryexpr) = 0;
    virtual void visitBinaryExpr(BinaryExpr &binaryexpr) = 0;
    virtual void visitAssignExpr(AssignExpr &assignexpr) = 0;
    virtual void visitEmptyExpr(EmptyExpr &emptyexpr) = 0;

    virtual void visitExprStmt(ExprStmt &exprstmt) = 0;
    virtual void visitBlockStmt(BlockStmt &blockstmt) = 0;
    virtual void visitIfStmt(IfStmt &ifstmt) = 0;
    virtual void visitElseStmt(ElseStmt &elsestmt) = 0;
    virtual void visitWhileStmt(WhileStmt &whilestmt) = 0;
    virtual void visitReturnStmt(ReturnStmt &returnstmt) = 0;
    virtual void visitDeclStmt(DeclStmt &declstmt) = 0;
    virtual void visitEmptyStmt(EmptyStmt &emptystmt) = 0;

    virtual void visitStructDecl(StructDecl &structdecl) = 0;
    virtual void visitStructField(StructField &structfield) = 0;

    virtual void visitParameter(Parameter &parameter) = 0;
    virtual void visitPrototype(Prototype &prototype) = 0;
    virtual void visitFuncDef(FuncDef &funcdef) = 0;

    virtual void visitProgram(Program &program) = 0;
};

class PrintVisitor : public Visitor {
  public:
    void visitIntExpr(IntExpr &intexpr);
    void visitCharExpr(CharExpr &charexpr);
    void visitVarExpr(VarExpr &varexpr);
    void visitCallExpr(CallExpr &callexpr);
    void visitCastExpr(CastExpr &castexpr);
    void visitDerefExpr(DerefExpr &derefexpr);
    void visitAddressExpr(AddressExpr &addressexpr);
    void visitUnaryExpr(UnaryExpr &unaryexpr);
    void visitBinaryExpr(BinaryExpr &binaryexpr);
    void visitAssignExpr(AssignExpr &assignexpr);
    void visitEmptyExpr(EmptyExpr &emptyexpr);

    void visitExprStmt(ExprStmt &exprstmt);
    void visitBlockStmt(BlockStmt &blockstmt);
    void visitIfStmt(IfStmt &ifstmt);
    void visitElseStmt(ElseStmt &elsestmt);
    void visitWhileStmt(WhileStmt &whilestmt);
    void visitReturnStmt(ReturnStmt &returnstmt);
    void visitDeclStmt(DeclStmt &declstmt);
    void visitEmptyStmt(EmptyStmt &emptystmt);

    void visitStructDecl(StructDecl &structdecl);
    void visitStructField(StructField &structfield);

    void visitParameter(Parameter &parameter);
    void visitPrototype(Prototype &prototype);
    void visitFuncDef(FuncDef &funcdef);

    void visitProgram(Program &program);

    std::string getIndent();
};

class SemanticVisitor : public Visitor {
  public:
    std::vector<Scope> scopeVec;
    TypeKind *currFuncRetType;
    int numOfErrors = 0;

    void visitIntExpr(IntExpr &intexpr);
    void visitCharExpr(CharExpr &charexpr);
    void visitVarExpr(VarExpr &varexpr);
    void visitCallExpr(CallExpr &callexpr);
    void visitCastExpr(CastExpr &castexpr);
    void visitDerefExpr(DerefExpr &derefexpr);
    void visitAddressExpr(AddressExpr &addressexpr);
    void visitUnaryExpr(UnaryExpr &unaryexpr);
    void visitBinaryExpr(BinaryExpr &binaryexpr);

    void handlePointerArithmetic(BinaryExpr &binaryexpr);

    void visitAssignExpr(AssignExpr &assignexpr);
    void visitEmptyExpr(EmptyExpr &emptyexpr);

    void visitExprStmt(ExprStmt &exprstmt);
    void visitBlockStmt(BlockStmt &blockstmt);
    void visitIfStmt(IfStmt &ifstmt);
    void visitElseStmt(ElseStmt &elsestmt);
    void visitWhileStmt(WhileStmt &whilestmt);
    void visitReturnStmt(ReturnStmt &returnstmt);
    void visitDeclStmt(DeclStmt &declstmt);
    void visitEmptyStmt(EmptyStmt &emptystmt);

    void visitStructDecl(StructDecl &structdecl);
    void visitStructField(StructField &structfield);

    void visitParameter(Parameter &parameter);
    void visitPrototype(Prototype &prototype);
    void visitFuncDef(FuncDef &funcdef);

    void visitProgram(Program &program);
};

#endif
