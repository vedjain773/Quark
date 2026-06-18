#include "Error.hpp"
#include "Expression.hpp"
#include "ExternalDecl.hpp"
#include "Function.hpp"
#include "Program.hpp"
#include "Statement.hpp"
#include "Visitor.hpp"
#include <cstddef>
#include <iostream>

using size_t = std::size_t;

void SemanticVisitor::visitProgram(Program &program) {
    Scope globalScope;
    scopeVec.push_back(globalScope);

    for (size_t i = 0; i < program.root.size(); i++) {
        ExternalDecl *edecl = (program.root[i]).get();
        edecl->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitParameter(Parameter &parameter) {
    scopeVec[scopeVec.size() - 1].addRow(parameter.name, parameter.type,
                                         SymbolKind::VARIABLE);
}

void SemanticVisitor::visitPrototype(Prototype &prototype) {

    if (scopeVec[0].search(prototype.funcName)) {
        Error error(prototype.line, prototype.column,
                    prototype.funcName + " is already declared");
        numOfErrors += 1;
    } else {
        scopeVec[0].addRow(prototype.funcName, prototype.retType,
                           SymbolKind::FUNCTION);
        currFuncRetType = prototype.retType;
    }

    for (size_t i = 0; i < prototype.paramList.size(); i++) {
        Parameter *param = (prototype.paramList[i]).get();
        param->accept(*this);
        scopeVec[0].addParam(prototype.funcName, param->type);
    }
}

void SemanticVisitor::visitFuncDef(FuncDef &funcdef) {
    Scope funcScope;
    scopeVec.push_back(funcScope);

    Prototype *proto = (funcdef.prototype).get();
    BlockStmt *body = (funcdef.funcBody).get();

    proto->accept(*this);

    for (size_t i = 0; i < body->statements.size(); i++) {
        Statement *statmt = (body->statements[i]).get();
        statmt->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitEmptyStmt(EmptyStmt &emptystmt) {
    // no checks
}

void SemanticVisitor::visitBlockStmt(BlockStmt &blockstmt) {
    Scope locScope;
    scopeVec.push_back(locScope);

    for (size_t i = 0; i < blockstmt.statements.size(); i++) {
        Statement *stmt = (blockstmt.statements[i]).get();
        stmt->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitDeclStmt(DeclStmt &declstmt) {
    if (scopeVec[scopeVec.size() - 1].search(declstmt.name)) {
        Error error(declstmt.line, declstmt.column,
                    declstmt.name + " is already declared");
        numOfErrors += 1;
    } else {
        scopeVec[scopeVec.size() - 1].addRow(declstmt.name, declstmt.type,
                                             SymbolKind::VARIABLE);
    }

    Expression *expr = (declstmt.expression).get();

    if (expr != nullptr) {
        expr->accept(*this);

        if (expr->infType == getType("void")) {
            Error error(declstmt.line, declstmt.column,
                        "Variables cannot be of type: void");
            numOfErrors += 1;
        }

        if (expr->infType != declstmt.type) {
            auto castexpr = std::make_unique<CastExpr>(
                std::move(declstmt.expression), expr->infType, declstmt.type);

            Expression *cexpr = castexpr.get();
            cexpr->accept(*this);

            declstmt.expression = std::move(castexpr);
        }

        declstmt.expression->infType = declstmt.type;
    }
}

void SemanticVisitor::visitIfStmt(IfStmt &ifstmt) {
    Expression *condn = (ifstmt.condition).get();
    Statement *ifbody = (ifstmt.body).get();
    Statement *elsestmt = (ifstmt.elseStmt).get();

    condn->accept(*this);

    if (condn->infType != getType("int")) {
        std::string typeName = condn->infType->name;
        Error error(ifstmt.line, ifstmt.column,
                    "Invalid (if) condition expression; Expected: int, Got: " +
                        typeName);
        numOfErrors += 1;
    }

    ifbody->accept(*this);

    if (elsestmt != nullptr) {
        elsestmt->accept(*this);
    }
}

void SemanticVisitor::visitElseStmt(ElseStmt &elsestmt) {
    Statement *elsebody = (elsestmt.body).get();

    elsebody->accept(*this);
}

void SemanticVisitor::visitWhileStmt(WhileStmt &whilestmt) {
    Expression *condn = (whilestmt.condition).get();
    Statement *whilebody = (whilestmt.body).get();

    condn->accept(*this);

    if (condn->infType != getType("int")) {
        std::string typeName = condn->infType->name;
        Error error(
            whilestmt.line, whilestmt.column,
            "Invalid (while) condition expression, Expected: int, Got: " +
                typeName);
        numOfErrors += 1;
    }

    whilebody->accept(*this);
}

void SemanticVisitor::visitReturnStmt(ReturnStmt &returnstmt) {
    Expression *retexpr = (returnstmt.retExpr).get();

    retexpr->accept(*this);

    if (retexpr->infType != currFuncRetType) {
        std::string retexprTypeName = retexpr->infType->name;
        std::string currRetTypeName = currFuncRetType->name;
        Error error(retexpr->line, retexpr->column,
                    "Return type (" + retexprTypeName +
                        ") does not match function signature (" +
                        currRetTypeName + ")");

        numOfErrors += 1;
    }
}

void SemanticVisitor::visitExprStmt(ExprStmt &exprstmt) {
    Expression *expr = (exprstmt.expression).get();

    expr->accept(*this);
}

void SemanticVisitor::visitEmptyExpr(EmptyExpr &emptyexpr) {
    emptyexpr.infType = getType("void");
}

void SemanticVisitor::visitAssignExpr(AssignExpr &assignexpr) {
    Expression *lExpr = (assignexpr.LHS).get();
    Expression *rExpr = (assignexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);

    if (rExpr->infType == getType("void")) {
        Error error(assignexpr.line, assignexpr.column,
                    "Assignment operand cannot be of Type: VOID");
        numOfErrors += 1;
        return;
    }

    if (lExpr->infType != rExpr->infType) {
        auto castexpr = std::make_unique<CastExpr>(std::move(assignexpr.RHS),
                                                   assignexpr.RHS->infType,
                                                   assignexpr.LHS->infType);

        Expression *cexpr = castexpr.get();
        cexpr->accept(*this);

        assignexpr.RHS = std::move(castexpr);
    }

    assignexpr.infType = assignexpr.LHS->infType;
}

void SemanticVisitor::visitBinaryExpr(BinaryExpr &binexpr) {
    Expression *lExpr = (binexpr.LHS).get();
    Expression *rExpr = (binexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);

    if (lExpr->infType == getType("void")) {
        Error error(lExpr->line, lExpr->column, "Binary Operand cannot be of type: void");
        numOfErrors += 1;
        return;
    }

    if (rExpr->infType == getType("void")) {
        Error error(rExpr->line, rExpr->column, "Binary Operand cannot be of type: void");
        numOfErrors += 1;
        return;
    } 

    if (lExpr->infType != rExpr->infType) {
        if (!isPointerType(lExpr->infType) && !isPointerType(rExpr->infType)) {
            auto castexpr = std::make_unique<CastExpr>(std::move(binexpr.RHS),
                                                       binexpr.RHS->infType,
                                                       binexpr.LHS->infType);

            Expression *cexpr = castexpr.get();
            cexpr->accept(*this);

            binexpr.RHS = std::move(castexpr);
        } else {
            TypeKind* typek = nullptr;

            if (isPointerType(lExpr->infType)) {
                typek = lExpr->infType;
            } else if (isPointerType(rExpr->infType)) {
                typek = rExpr->infType;
            }

            binexpr.infType = typek;
            return;
        }
    }

    binexpr.infType = binexpr.LHS->infType;
}

void SemanticVisitor::visitUnaryExpr(UnaryExpr &unaryexpr) {
    Expression *Operand = (unaryexpr.Operand).get();

    Operand->accept(*this);

    if (Operand->infType == getType("int")) {
        unaryexpr.infType = getType("int");
    } else {
        std::string typeName = Operand->infType->name;
        Error error(unaryexpr.line, unaryexpr.column,
                    "Operand must be of type: int, Got: " + typeName);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitDerefExpr(DerefExpr &derefexpr) {
    Expression *expr = (derefexpr.expr).get();

    expr->accept(*this);

    if (!isPointerType(expr->infType)) {
        std::string typeName = expr->infType->name;
        Error error(derefexpr.line, derefexpr.column,
                    "Expected: POINTER, Got: " + typeName);
        numOfErrors += 1;
    }

    derefexpr.infType = expr->infType->to;
}

void SemanticVisitor::visitAddressExpr(AddressExpr &addressexpr) {
    Expression *expr = (addressexpr.expr).get();

    if (!expr->isLValue()) {
        Error error(addressexpr.line, addressexpr.column,
                    "Operand must be an lvalue");
        numOfErrors += 1;
    }

    expr->accept(*this);

    std::string typeName = expr->infType->name;
    addressexpr.infType = getType(typeName + '*');
}

void SemanticVisitor::visitCastExpr(CastExpr &castexpr) {
    castexpr.infType = castexpr.to;
}

void SemanticVisitor::visitCallExpr(CallExpr &callexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(callexpr.callee)) {
            flag = true;

            if (scopeVec[i].getSymKind(callexpr.callee) ==
                SymbolKind::FUNCTION) {
                callexpr.infType = scopeVec[i].getSymType(callexpr.callee);
                break;
            } else {
                Error error(callexpr.line, callexpr.column,
                            callexpr.callee + " is not a callable function");
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(callexpr.line, callexpr.column,
                    "Undeclared function: " + callexpr.callee);
        numOfErrors += 1;
    }

    if (scopeVec[0].getNumParams(callexpr.callee) != callexpr.args.size()) {
        int expected = scopeVec[0].getNumParams(callexpr.callee);
        int got = callexpr.args.size();

        Error error(callexpr.line, callexpr.column,
                    "Expected " + std::to_string(expected) +
                        " arguments, got: " + std::to_string(got));
        numOfErrors += 1;
    }

    for (size_t i = 0; i < callexpr.args.size(); i++) {
        Expression *expr = (callexpr.args[i]).get();
        expr->accept(*this);
    }
}

void SemanticVisitor::visitVarExpr(VarExpr &varexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(varexpr.Name)) {
            flag = true;

            if (scopeVec[i].getSymKind(varexpr.Name) == SymbolKind::VARIABLE) {
                varexpr.infType = scopeVec[i].getSymType(varexpr.Name);
                break;
            } else {
                Error error(varexpr.line, varexpr.column,
                            varexpr.Name + "is not a variable");
                numOfErrors += 1;
            }

            break;
        }
    }

    if (!flag) {
        Error error(varexpr.line, varexpr.column,
                    "Undeclared variable: " + varexpr.Name);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitCharExpr(CharExpr &charexpr) {
    charexpr.infType = getType("char");
}

void SemanticVisitor::visitIntExpr(IntExpr &intexpr) {
    intexpr.infType = getType("int");
}
