#include "Visitor.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Program.hpp"
#include "Error.hpp"
#include "Function.hpp"
#include "ExternalDecl.hpp"
#include <iostream>

void SemanticVisitor::visitProgram(Program& program) {
    Scope globalScope;
    scopeVec.push_back(globalScope);

    for (int i = 0; i < program.root.size(); i++) {
        ExternalDecl* edecl = (program.root[i]).get();
        edecl->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitParameter(Parameter& parameter) {
    scopeVec[scopeVec.size() - 1].addRow(parameter.name, parameter.type, SymbolKind::VARIABLE);
}

void SemanticVisitor::visitPrototype(Prototype& prototype) {

    if (scopeVec[0].search(prototype.funcName)) {
        Error error(prototype.line, prototype.column, prototype.funcName + " is already declared");
        printErrorMsg(error);
        numOfErrors += 1;
    } else {
        scopeVec[0].addRow(prototype.funcName, prototype.retType, SymbolKind::FUNCTION);
        currFuncRetType = prototype.retType;
    }

    for (int i = 0; i < prototype.paramList.size(); i++) {
        Parameter* param = (prototype.paramList[i]).get();
        param->accept(*this);
        scopeVec[0].addParam(prototype.funcName, param->type);
    }
}

void SemanticVisitor::visitFuncDef(FuncDef& funcdef) {
    Scope funcScope;
    scopeVec.push_back(funcScope);

    Prototype* proto = (funcdef.prototype).get();
    BlockStmt* body = (funcdef.funcBody).get();

    proto->accept(*this);

    for (int i = 0; i < body->statements.size(); i++) {
        Statement* statmt = (body->statements[i]).get();
        statmt->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitEmptyStmt(EmptyStmt& emptystmt) {
    //no checks
}

void SemanticVisitor::visitBlockStmt(BlockStmt& blockstmt) {
    Scope locScope;
    scopeVec.push_back(locScope);

    for (int i = 0; i < blockstmt.statements.size(); i++) {
        Statement* stmt = (blockstmt.statements[i]).get();
        stmt->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitDeclStmt(DeclStmt& declstmt) {
    if (scopeVec[scopeVec.size() - 1].search(declstmt.name)) {
        Error error(declstmt.line, declstmt.column, declstmt.name + " is already declared");
        printErrorMsg(error);
        numOfErrors += 1;
    } else {
        scopeVec[scopeVec.size() - 1].addRow(declstmt.name, declstmt.type, SymbolKind::VARIABLE);
    }

    Expression* expr = (declstmt.expression).get();

    if (expr != nullptr) {
        expr->accept(*this);

        if (expr->infType == TypeKind::VOID) {
            Error error(declstmt.line, declstmt.column, "Variables cannot be of type: VOID");
            printErrorMsg(error);
            numOfErrors += 1;
        }
    }
}

void SemanticVisitor::visitIfStmt(IfStmt& ifstmt) {
    Expression* condn = (ifstmt.condition).get();
    Statement* ifbody = (ifstmt.body).get();
    Statement* elsestmt = (ifstmt.elseStmt).get();

    condn->accept(*this);

    if (condn->infType != TypeKind::INT) {
        Error error(ifstmt.line, ifstmt.column, "Invalid (while) condition expression");
        printErrorMsg(error);
        numOfErrors += 1;
    }

    ifbody->accept(*this);

    if (elsestmt != nullptr) {
        elsestmt->accept(*this);
    }
}

void SemanticVisitor::visitElseStmt(ElseStmt& elsestmt) {
    Statement* elsebody = (elsestmt.body).get();

    elsebody->accept(*this);
}

void SemanticVisitor::visitWhileStmt(WhileStmt& whilestmt) {
    Expression* condn = (whilestmt.condition).get();
    Statement* whilebody = (whilestmt.body).get();

    condn->accept(*this);

    if (condn->infType != TypeKind::INT) {
        Error error(whilestmt.line, whilestmt.column, "Invalid (while) condition expression");
        printErrorMsg(error);
        numOfErrors += 1;
    }

    whilebody->accept(*this);
}

void SemanticVisitor::visitReturnStmt(ReturnStmt& returnstmt) {
    Expression* retexpr = (returnstmt.retExpr).get();

    retexpr->accept(*this);

    if (retexpr->infType != TypeKind::VOID) {
        if (currFuncRetType == TypeKind::VOID) {
            Error error(retexpr->line, retexpr->column, "Void function cannot return a literal");
            printErrorMsg(error);
            numOfErrors += 1;
        }
    } else {
        if (currFuncRetType != TypeKind::VOID) {
            Error error(retexpr->line, retexpr->column, "Non-void function must return a literal");
            printErrorMsg(error);
            numOfErrors += 1;
        }
    }
}

void SemanticVisitor::visitExprStmt(ExprStmt& exprstmt) {
    Expression* expr = (exprstmt.expression).get();

    expr->accept(*this);
}

void SemanticVisitor::visitEmptyExpr(EmptyExpr& emptyexpr) {
    emptyexpr.infType = TypeKind::VOID;
}

void SemanticVisitor::visitAssignExpr(AssignExpr& assignexpr) {
    Expression* lExpr = (assignexpr.LHS).get();
    Expression* rExpr = (assignexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);

    if (lExpr->infType != TypeKind::VOID && rExpr->infType != TypeKind::VOID) {
        assignexpr.infType = TypeKind::INT;
    } else {
        Error error(assignexpr.line, assignexpr.column, "Assignment operand cannot be of Type: VOID");
        printErrorMsg(error);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitBinaryExpr(BinaryExpr& binexpr) {
    Expression* lExpr = (binexpr.LHS).get();
    Expression* rExpr = (binexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);

    if (lExpr->infType != TypeKind::VOID && rExpr->infType != TypeKind::VOID) {
        binexpr.infType = TypeKind::INT;
    } else {
        Error error(binexpr.line, binexpr.column, "Binary operand cannot be of Type: VOID");
        printErrorMsg(error);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitUnaryExpr(UnaryExpr& unaryexpr) {
    Expression* Operand = (unaryexpr.Operand).get();

    Operand->accept(*this);

    if (Operand->infType == TypeKind::INT) {
        unaryexpr.infType = TypeKind::INT;
    } else {
        Error error(unaryexpr.line, unaryexpr.column, "Operand must be of type: INT");
        printErrorMsg(error);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitCallExpr(CallExpr& callexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(callexpr.callee)) {
            flag = true;

            if (scopeVec[i].getSymKind(callexpr.callee) == SymbolKind::FUNCTION) {
                callexpr.infType = scopeVec[i].getSymType(callexpr.callee);
                break;
            } else {
                Error error(callexpr.line, callexpr.column, callexpr.callee + " is not a callable function");
                printErrorMsg(error);
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(callexpr.line, callexpr.column, "Undeclared function: " + callexpr.callee);
        printErrorMsg(error);
        numOfErrors += 1;
    }

    if (scopeVec[0].getNumParams(callexpr.callee) != callexpr.args.size()) {
        int expected = scopeVec[0].getNumParams(callexpr.callee);
        int got = callexpr.args.size();

        Error error(callexpr.line, callexpr.column, "Expected " + std::to_string(expected) + " arguments, got: " + std::to_string(got));
        printErrorMsg(error);
        numOfErrors += 1;
    }

    for (int i = 0; i < callexpr.args.size(); i++) {
        Expression* expr = (callexpr.args[i]).get();
        expr->accept(*this);
    }
}

void SemanticVisitor::visitVarExpr(VarExpr& varexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(varexpr.Name)) {
            flag = true;
            break;

            if (scopeVec[i].getSymKind(varexpr.Name) == SymbolKind::VARIABLE) {
                varexpr.infType = scopeVec[i].getSymType(varexpr.Name);
                break;
            } else {
                Error error(varexpr.line, varexpr.column, varexpr.Name + "is not a variable");
                printErrorMsg(error);
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(varexpr.line, varexpr.column, "Undeclared variable: " + varexpr.Name);
        printErrorMsg(error);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitCharExpr(CharExpr& charexpr) {
    charexpr.infType = TypeKind::CHAR;
}

void SemanticVisitor::visitIntExpr(IntExpr& intexpr) {
    intexpr.infType = TypeKind::INT;
}
