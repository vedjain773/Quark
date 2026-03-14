#include "Visitor.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Program.hpp"
#include "Error.hpp"
#include "Function.hpp"
#include "ExternalDecl.hpp"
#include <iostream>

void PrintVisitor::visitIntExpr(IntExpr& intexpr) {
    std::cout << getIndent() << "|-Int(" << intexpr.Val << ")\n";
}

void PrintVisitor::visitCharExpr(CharExpr& charexpr) {
    std::cout << getIndent() << "|-Char(" << charexpr.character << ")\n";
}

void PrintVisitor::visitVarExpr(VarExpr& varexpr) {
    std::cout << getIndent() << "|-Var(" << varexpr.Name << ")\n";
}

void PrintVisitor::visitCallExpr(CallExpr& callexpr) {
    std::cout << getIndent() << "|-Call(" << callexpr.callee << ")\n";

    depth += 1;
    for (int i = 0; i < callexpr.args.size(); i++) {
        Expression* expr = (callexpr.args[i]).get();
        expr->accept(*this);
    }
    depth -= 1;
}

void PrintVisitor::visitUnaryExpr(UnaryExpr& unaryexpr) {
    std::cout << getIndent() << "|-Unary(" << getOpStr(unaryexpr.Op) << ")\n";

    Expression* Operand = (unaryexpr.Operand).get();

    depth += 1;
    Operand->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitBinaryExpr(BinaryExpr& binexpr) {
    std::cout << getIndent() << "|-Oper(" << getOpStr(binexpr.Op) << ")\n";

    Expression* lExpr = (binexpr.LHS).get();
    Expression* rExpr = (binexpr.RHS).get();

    depth += 1;
    lExpr->accept(*this);
    depth -= 1;

    depth += 1;
    rExpr->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitAssignExpr(AssignExpr& assignexpr) {
    std::cout << getIndent() << "|-Assign(=)\n";

    Expression* lExpr = (assignexpr.LHS).get();
    Expression* rExpr = (assignexpr.RHS).get();

    depth += 1;
    lExpr->accept(*this);
    depth -= 1;

    depth += 1;
    rExpr->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitEmptyExpr(EmptyExpr& emptyexpr) {
    //ignore
}

void PrintVisitor::visitExprStmt(ExprStmt& exprstmt) {
    std::cout << getIndent() << "|-Stmt(Expr)\n";

    Expression* expr = (exprstmt.expression).get();

    depth += 1;
    expr->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitBlockStmt(BlockStmt& blockstmt) {
    std::cout << getIndent() << "|-Stmt(Block)\n";

    depth += 1;
    for (int i = 0; i < blockstmt.statements.size(); i++) {
        Statement* stmt = (blockstmt.statements[i]).get();
        stmt->accept(*this);
    }
    depth -= 1;
}

void PrintVisitor::visitIfStmt(IfStmt& ifstmt) {
    std::cout << getIndent() << "|-Stmt(If)\n";

    Expression* condn = (ifstmt.condition).get();
    Statement* ifbody = (ifstmt.body).get();
    Statement* elsestmt = (ifstmt.elseStmt).get();

    depth += 1;

    condn->accept(*this);
    ifbody->accept(*this);

    if (elsestmt != nullptr) {
        elsestmt->accept(*this);
    }

    depth -= 1;
}

void PrintVisitor::visitElseStmt(ElseStmt& elsestmt) {
    std::cout << getIndent() << "|-Stmt(Else)\n";

    Statement* elsebody = (elsestmt.body).get();

    depth += 1;
    elsebody->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitWhileStmt(WhileStmt& whilestmt) {
    std::cout << getIndent() << "|-Stmt(While)\n";

    Expression* condn = (whilestmt.condition).get();
    Statement* whilebody = (whilestmt.body).get();

    depth += 1;
    condn->accept(*this);
    whilebody->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitReturnStmt(ReturnStmt& returnstmt) {
    std::cout << getIndent() << "|-Stmt(Return)\n";

    Expression* retexpr = (returnstmt.retExpr).get();

    depth += 1;
    retexpr->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitDeclStmt(DeclStmt& declstmt) {
    std::cout << getIndent() << "|-Stmt(Declare)\n";
    std::cout << getIndent() << "  |-Var(" + declstmt.name + ")\n";

    Expression* expr = (declstmt.expression).get();

    depth += 1;

    if (expr != nullptr) {
        expr->accept(*this);
    }

    depth -= 1;
}

void PrintVisitor::visitEmptyStmt(EmptyStmt& emptystmt) {
    std::cout << getIndent() << "\n";
}

void PrintVisitor::visitParameter(Parameter& parameter) {
    std::cout << getIndent() << "|-Param(" << parameter.name << ")\n";
}

void PrintVisitor::visitPrototype(Prototype& prototype) {
    std::cout << getIndent() << "|-Prototype(" << prototype.funcName << ")\n";

    depth += 1;
    for (int i = 0; i < prototype.paramList.size(); i++) {
        Parameter* param = (prototype.paramList[i]).get();
        param->accept(*this);
    }
    depth -= 1;
}

void PrintVisitor::visitFuncDef(FuncDef& funcdef) {
    std::cout << getIndent() << "|-FuncDef\n";

    Prototype* proto = (funcdef.prototype).get();
    Statement* stmt = (funcdef.funcBody).get();

    depth += 1;
    proto->accept(*this);
    stmt->accept(*this);
    depth -= 1;
}

void PrintVisitor::visitProgram(Program& program) {
    std::cout << getIndent() << "\n";

    for (int i = 0; i < program.root.size(); i++) {
        ExternalDecl* edecl = (program.root[i]).get();
        edecl->accept(*this);
    }
}

std::string PrintVisitor::getIndent() {
    std::string indent = "";

    for (int i = 0; i < depth; i++) {
        indent.append("  ");
    }

    return indent;
}

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
        Error error(prototype.line, prototype.column);
        error.printErrorMsg(prototype.funcName + " is already declared");
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
        Error error(declstmt.line, declstmt.column);
        error.printErrorMsg(declstmt.name + " is already declared");
        numOfErrors += 1;
    } else {
        scopeVec[scopeVec.size() - 1].addRow(declstmt.name, declstmt.type, SymbolKind::VARIABLE);
    }

    Expression* expr = (declstmt.expression).get();

    if (expr != nullptr) {
        expr->accept(*this);

        if (expr->infType == TypeKind::VOID) {
            Error error(declstmt.line, declstmt.column);
            error.printErrorMsg("Variables cannot be of type: VOID");
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
        Error error(ifstmt.line, ifstmt.column);
        error.printErrorMsg("Invalid (while) condition expression");
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
        Error error(whilestmt.line, whilestmt.column);
        error.printErrorMsg("Invalid (while) condition expression");
        numOfErrors += 1;
    }

    whilebody->accept(*this);
}

void SemanticVisitor::visitReturnStmt(ReturnStmt& returnstmt) {
    Expression* retexpr = (returnstmt.retExpr).get();

    retexpr->accept(*this);

    if (retexpr->infType != TypeKind::VOID) {
        if (currFuncRetType == TypeKind::VOID) {
            Error error(retexpr->line, retexpr->column);
            error.printErrorMsg("Void function cannot return a literal");
            numOfErrors += 1;
        }
    } else {
        if (currFuncRetType != TypeKind::VOID) {
            Error error(retexpr->line, retexpr->column);
            error.printErrorMsg("Non-void function must return a literal");
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
        Error error(assignexpr.line, assignexpr.column);
        error.printErrorMsg("Assignment operand cannot be of Type: VOID");
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
        Error error(binexpr.line, binexpr.column);
        error.printErrorMsg("Binary operand cannot be of Type: VOID");
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitUnaryExpr(UnaryExpr& unaryexpr) {
    Expression* Operand = (unaryexpr.Operand).get();

    Operand->accept(*this);

    if (Operand->infType == TypeKind::INT) {
        unaryexpr.infType = TypeKind::INT;
    } else {
        Error error(unaryexpr.line, unaryexpr.column);
        error.printErrorMsg("Operand must be of type: INT");
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
                Error error(callexpr.line, callexpr.column);
                error.printErrorMsg(callexpr.callee + " is not a callable function");
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(callexpr.line, callexpr.column);
        error.printErrorMsg("Undeclared function: " + callexpr.callee);
        numOfErrors += 1;
    }

    if (scopeVec[0].getNumParams(callexpr.callee) != callexpr.args.size()) {
        int expected = scopeVec[0].getNumParams(callexpr.callee);
        int got = callexpr.args.size();

        Error error(callexpr.line, callexpr.column);
        error.printErrorMsg(
            "Expected " + std::to_string(expected) +
            " arguments, got: " + std::to_string(got)
        );
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
                Error error(varexpr.line, varexpr.column);
                error.printErrorMsg(varexpr.Name + "is not a variable");
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(varexpr.line, varexpr.column);
        error.printErrorMsg("Undeclared variable: " + varexpr.Name);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitCharExpr(CharExpr& charexpr) {
    charexpr.infType = TypeKind::CHAR;
}

void SemanticVisitor::visitIntExpr(IntExpr& intexpr) {
    intexpr.infType = TypeKind::INT;
}
