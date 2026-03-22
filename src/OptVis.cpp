#include "OptVis.hpp"
#include "Visitor.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Program.hpp"
#include "Error.hpp"
#include "Function.hpp"
#include "ExternalDecl.hpp"
#include <iostream>

std::unique_ptr<Program> OptimizeVisitor::visitProgram(Program& program) {
    auto optProg = std::make_unique<Program>();

    for (int i = 0; i < program.root.size(); i++) {
        ExternalDecl* edecl = (program.root[i]).get();
        auto optEdecl = edecl->optimize(*this);

        optProg->add(std::move(optEdecl));
    }

    return std::move(optProg);
}

std::unique_ptr<Parameter> OptimizeVisitor::visitParameter(Parameter& parameter) {
    auto optParam = std::make_unique<Parameter>(parameter.type, parameter.name);
    return optParam;
}

std::unique_ptr<Prototype> OptimizeVisitor::visitPrototype(Prototype& prototype) {
    auto optProto = std::make_unique<Prototype>(prototype.retType, prototype.funcName, prototype.line, prototype.column);

    for (int i = 0; i < prototype.paramList.size(); i++) {
        Parameter* param = (prototype.paramList[i]).get();
        auto optParam = param->optimize(*this);

        optProto->addParam(std::move(optParam));
    }

    return std::move(optProto);
}

std::unique_ptr<FuncDef> OptimizeVisitor::visitFuncDef(FuncDef& funcdef) {
    Prototype* proto = (funcdef.prototype).get();
    BlockStmt* body = (funcdef.funcBody).get();

    auto optBody = std::make_unique<BlockStmt>();
    auto optProto = proto->optimize(*this);

    for (int i = 0; i < body->statements.size(); i++) {
        Statement* statmt = (body->statements[i]).get();
        auto optStmt = statmt->optimize(*this);

        optBody->addStmt(std::move(optStmt));
    }

    return std::make_unique<FuncDef>(std::move(optProto), std::move(optBody));
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(EmptyStmt& emptystmt) {
    return std::make_unique<EmptyStmt>();
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(BlockStmt& blockstmt) {
    auto optBlock = std::make_unique<BlockStmt>();

    for (int i = 0; i < blockstmt.statements.size(); i++) {
        Statement* stmt = (blockstmt.statements[i]).get();
        auto optStmt = stmt->optimize(*this);

        optBlock->addStmt(std::move(optStmt));
    }

    return std::move(optBlock);
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(DeclStmt& declstmt) {
    Expression* expr = (declstmt.expression).get();

    if (expr != nullptr) {
        auto optExpr = expr->optimize(*this);
        return std::make_unique<DeclStmt>(declstmt.type, declstmt.name, std::move(optExpr), declstmt.line, declstmt.column);
    } else {
        return std::make_unique<DeclStmt>(declstmt.type, declstmt.name, nullptr, declstmt.line, declstmt.column);
    }
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(IfStmt& ifstmt) {
    Expression* condn = (ifstmt.condition).get();
    Statement* ifbody = (ifstmt.body).get();
    Statement* elsestmt = (ifstmt.elseStmt).get();

    auto optCond = condn->optimize(*this);

    auto optBody = ifbody->optimize(*this);

    if (elsestmt != nullptr) {
        auto optElse = elsestmt->optimize(*this);
        return std::make_unique<IfStmt>(std::move(optCond), std::move(optBody), std::move(optElse));
    }

    return std::make_unique<IfStmt>(std::move(optCond), std::move(optBody), nullptr);
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(ElseStmt& elsestmt) {
    Statement* elsebody = (elsestmt.body).get();

    auto optBody = elsebody->optimize(*this);
    return std::make_unique<ElseStmt>(std::move(optBody));
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(WhileStmt& whilestmt) {
    Expression* condn = (whilestmt.condition).get();
    Statement* whilebody = (whilestmt.body).get();

    auto optCond = condn->optimize(*this);

    auto optBody = whilebody->optimize(*this);

    return std::make_unique<WhileStmt>(std::move(optCond), std::move(optBody));
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(ReturnStmt& returnstmt) {
    Expression* retexpr = (returnstmt.retExpr).get();

    auto optExpr = retexpr->optimize(*this);

    return std::make_unique<ReturnStmt>(std::move(optExpr));
}

std::unique_ptr<Statement> OptimizeVisitor::visitStmt(ExprStmt& exprstmt) {
    Expression* expr = (exprstmt.expression).get();

    auto optExpr = expr->optimize(*this);

    return std::make_unique<ExprStmt>(std::move(optExpr));
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(EmptyExpr& emptyexpr) {
    return std::make_unique<EmptyExpr>();
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(AssignExpr& assignexpr) {
    Expression* lExpr = (assignexpr.LHS).get();
    Expression* rExpr = (assignexpr.RHS).get();

    auto optLExpr = lExpr->optimize(*this);

    auto optRExpr = rExpr->optimize(*this);

    return std::make_unique<AssignExpr>(std::move(optLExpr), std::move(optRExpr), assignexpr.line, assignexpr.column);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(BinaryExpr& binexpr) {
    Expression* lExpr = (binexpr.LHS).get();
    Expression* rExpr = (binexpr.RHS).get();

    auto optLExpr = lExpr->optimize(*this);

    auto optRExpr = rExpr->optimize(*this);

    if (optLExpr->getNodeType() == NodeType::INT_EXPR && optRExpr->getNodeType() == NodeType::INT_EXPR) {

        int line = optLExpr->line;
        int column = optLExpr->column;
        int value = perfOp(getIntVal(std::move(optLExpr)), getIntVal(std::move(optRExpr)), binexpr.Op);
        return std::make_unique<IntExpr>(value, line, column);

    } else if (optLExpr->getNodeType() == NodeType::CHAR_EXPR && optRExpr->getNodeType() == NodeType::CHAR_EXPR) {

        int line = optLExpr->line;
        int column = optLExpr->column;
        int value = perfOp(getCharVal(std::move(optLExpr)), getCharVal(std::move(optRExpr)), binexpr.Op);
        return std::make_unique<CharExpr>(value, line, column);

    } else if (optLExpr->getNodeType() == NodeType::INT_EXPR) {

        if (isIdentityVal(binexpr.Op, true, getIntVal(std::move(optLExpr)))) {
            return std::move(optRExpr);
        }

    } else if (optRExpr->getNodeType() == NodeType::INT_EXPR) {

        if (isIdentityVal(binexpr.Op, false, getIntVal(std::move(optRExpr)))) {
            return std::move(optLExpr);
        }

    }

    return std::make_unique<BinaryExpr>(binexpr.Op, std::move(optLExpr), std::move(optRExpr), binexpr.line, binexpr.column);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(UnaryExpr& unaryexpr) {
    Expression* Operand = (unaryexpr.Operand).get();

    auto optOper = Operand->optimize(*this);

    return std::make_unique<UnaryExpr>(unaryexpr.Op, std::move(optOper), unaryexpr.line, unaryexpr.column);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(CallExpr& callexpr) {
    auto optCallExpr = std::make_unique<CallExpr>(callexpr.callee, callexpr.line, callexpr.column);

    for (int i = 0; i < callexpr.args.size(); i++) {
        Expression* expr = (callexpr.args[i]).get();
        auto optExpr = expr->optimize(*this);

        optCallExpr->add(std::move(optExpr));
    }

    return std::move(optCallExpr);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(VarExpr& varexpr) {
    return std::make_unique<VarExpr>(varexpr.Name, varexpr.line, varexpr.column);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(CharExpr& charexpr) {
    return std::make_unique<CharExpr>(charexpr.character, charexpr.line, charexpr.column);
}

std::unique_ptr<Expression> OptimizeVisitor::visitExpr(IntExpr& intexpr) {
    return std::make_unique<IntExpr>(intexpr.Val, intexpr.line, intexpr.column);
}

int OptimizeVisitor::getIntVal(std::unique_ptr<Expression> intexpr) {
    IntExpr* intexpr_r = static_cast<IntExpr*>(intexpr.get());
    return intexpr_r->Val;
}

char OptimizeVisitor::getCharVal(std::unique_ptr<Expression> charexpr) {
    CharExpr* charexpr_r = static_cast<CharExpr*>(charexpr.get());
    return charexpr_r->character;
}

int OptimizeVisitor::perfOp(int val1, int val2, Operators Op) {
    switch(Op) {
        case Operators::PLUS: return val1 + val2;
        case Operators::MINUS: return val1 - val2;
        case Operators::MULT: return val1 * val2;
        case Operators::DIVIDE: return val1 / val2;
        case Operators::MODULUS: return val1 % val2;
        case Operators::GREATER: return val1 > val2;
        case Operators::GREATER_EQUALS: return val1 >= val2;
        case Operators::LESS: return val1 < val2;
        case Operators::LESS_EQUALS: return val1 <= val2;
        case Operators::EQUALS: return val1 == val2;
        case Operators::NOT_EQUALS: return val1 != val2;
        case Operators::AND: return val1 && val2;
        case Operators::OR: return val1 || val2;
        default: return val1;
    }
}

char OptimizeVisitor::perfOp(char val1, char val2, Operators Op) {
    switch(Op) {
        case Operators::PLUS: return val1 + val2;
        case Operators::MINUS: return val1 - val2;
        case Operators::MULT: return val1 * val2;
        case Operators::DIVIDE: return val1 / val2;
        case Operators::MODULUS: return val1 % val2;
        case Operators::GREATER: return val1 > val2;
        case Operators::GREATER_EQUALS: return val1 >= val2;
        case Operators::LESS: return val1 < val2;
        case Operators::LESS_EQUALS: return val1 <= val2;
        case Operators::EQUALS: return val1 == val2;
        case Operators::NOT_EQUALS: return val1 != val2;
        case Operators::AND: return val1 && val2;
        case Operators::OR: return val1 || val2;
        default: return val1;
    }
}

bool OptimizeVisitor::isIdentityVal(Operators Op, bool left, int val) {
    switch (Op) {
        case Operators::PLUS: return val == 0;
        case Operators::MINUS: return (left ? 0 : val == 0);
        case Operators::MULT: return val == 1;
        case Operators::DIVIDE: return (left ? 0 : val == 1);
        default: return 0;
    }
}
