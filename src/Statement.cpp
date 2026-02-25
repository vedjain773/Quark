#include "Statement.hpp"
#include "Visitor.hpp"

void EmptyStmt::accept(Visitor& visitor) {
    visitor.visitEmptyStmt(*this);
}

void EmptyStmt::codegen(CodegenVis& codegenvis) {
    //do nothing
}

ExprStmt::ExprStmt(std::unique_ptr<Expression> expr) {
    expression = std::move(expr);

    Expression* exprn = expression.get();
    line = exprn->line;
    column = exprn->column;
}

void ExprStmt::accept(Visitor& visitor) {
    visitor.visitExprStmt(*this);
}

void ExprStmt::codegen(CodegenVis& codegenvis) {
    expression->codegen(codegenvis);
}

void BlockStmt::addStmt(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));

    Statement* statmt = (statements[0]).get();
    line = statmt->line;
    column = statmt->column;
}

void BlockStmt::accept(Visitor& visitor) {
    visitor.visitBlockStmt(*this);
}

void BlockStmt::codegen(CodegenVis& codegenvis) {
    for (int i = 0; i < statements.size(); i++) {
        statements[i]->codegen(codegenvis);
    }
}

IfStmt::IfStmt(std::unique_ptr<Expression> condn, std::unique_ptr<Statement> ifbody, std::unique_ptr<Statement> elsestmt) {
    condition = std::move(condn);
    body = std::move(ifbody);
    elseStmt = std::move(elsestmt);

    Expression* expr = condition.get();
    line = expr->line;
    column = expr->column;
}

void IfStmt::accept(Visitor& visitor) {
    visitor.visitIfStmt(*this);
}

void IfStmt::codegen(CodegenVis& codegenvis) {

}

ElseStmt::ElseStmt(std::unique_ptr<Statement> elsebody) {
    body = std::move(elsebody);

    Statement* statmt = body.get();
    line = statmt->line;
    column = statmt->column;
}

void ElseStmt::accept(Visitor& visitor) {
    visitor.visitElseStmt(*this);
}

void ElseStmt::codegen(CodegenVis& codegen) {

}

WhileStmt::WhileStmt(std::unique_ptr<Expression> condn, std::unique_ptr<Statement> whilebody) {
    condition = std::move(condn);
    body = std::move(whilebody);

    Expression* expr = condition.get();
    line = expr->line;
    column = expr->column;
}

void WhileStmt::accept(Visitor& visitor) {
    visitor.visitWhileStmt(*this);
}

void WhileStmt::codegen(CodegenVis& codegenvis) {

}

ReturnStmt::ReturnStmt(std::unique_ptr<Expression> retexpr) {
    retExpr = std::move(retexpr);

    Expression* expr = retExpr.get();
    line = expr->line;
    column = expr->column;
}

void ReturnStmt::accept(Visitor& visitor) {
    visitor.visitReturnStmt(*this);
}

void ReturnStmt::codegen(CodegenVis& codegenvis) {
    llvm::Value* retVal = retExpr->codegen(codegenvis);
    codegenvis.Builder->CreateRet(retVal);
}

DeclStmt::DeclStmt(TokenType tokentype, std::string varname, std::unique_ptr<Expression> expr, int tline, int tcol) {
    type = tokentype;
    name = varname;
    expression = std::move(expr);
    line = tline;
    column = tcol;
}

void DeclStmt::accept(Visitor& visitor) {
    visitor.visitDeclStmt(*this);
}

void DeclStmt::codegen(CodegenVis& codegenvis) {
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();
    llvm::Function* func = Bldr->GetInsertBlock()->getParent();

    llvm::AllocaInst* alloca = codegenvis.CreateEntryBlockAlloca(func, name, TokToType(type));
    llvm::Value* initVal = expression->codegen(codegenvis);

    Bldr->CreateStore(initVal, alloca);
    codegenvis.insertName(name, alloca);
}
