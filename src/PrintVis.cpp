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

        if (stmt != nullptr)
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
