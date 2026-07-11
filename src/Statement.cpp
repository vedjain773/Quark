#include "Statement.hpp"
#include "Visitor.hpp"

void EmptyStmt::accept(Visitor &visitor) { visitor.visitEmptyStmt(*this); }

void EmptyStmt::codegen(CodegenVis &codegenvis) {
    // do nothing
}

ExprStmt::ExprStmt(std::unique_ptr<Expression> expr) {
    expression = std::move(expr);

    Expression *exprn = expression.get();
    line = exprn->line;
    column = exprn->column;
}

void ExprStmt::accept(Visitor &visitor) { visitor.visitExprStmt(*this); }

void ExprStmt::codegen(CodegenVis &codegenvis) {
    expression->codegen(codegenvis);
}

void BlockStmt::addStmt(std::unique_ptr<Statement> stmt) {
    statements.push_back(std::move(stmt));

    Statement *statmt = (statements[0]).get();

    if (statmt == nullptr)
        return;

    line = statmt->line;
    column = statmt->column;
}

void BlockStmt::accept(Visitor &visitor) { visitor.visitBlockStmt(*this); }

void BlockStmt::codegen(CodegenVis &codegenvis) {
    codegenvis.pushScope();
    for (size_t i = 0; i < statements.size(); i++) {
        statements[i]->codegen(codegenvis);

        if (statements[i]->isTerminator())
            break;
    }
    codegenvis.popScope();
}

IfStmt::IfStmt(std::unique_ptr<Expression> condn,
               std::unique_ptr<Statement> ifbody,
               std::unique_ptr<Statement> elsestmt) {
    condition = std::move(condn);
    body = std::move(ifbody);
    elseStmt = std::move(elsestmt);

    Expression *expr = condition.get();
    line = expr->line;
    column = expr->column;
}

void IfStmt::accept(Visitor &visitor) { visitor.visitIfStmt(*this); }

void IfStmt::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();
    llvm::Value *cond = condition->codegen(codegenvis);

    if (!cond) {
        return;
    }

    llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Cxt), 0);

    cond = Bldr->CreateICmpNE(cond, zero, "ifcond");

    llvm::Function *func = Bldr->GetInsertBlock()->getParent();

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(*Cxt, "then", func);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*Cxt, "ifcont");
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(*Cxt, "else");

    if (elseStmt != nullptr) {
        Bldr->CreateCondBr(cond, thenBB, elseBB);
    } else {
        Bldr->CreateCondBr(cond, thenBB, mergeBB);
    }

    Bldr->SetInsertPoint(thenBB);

    codegenvis.pushScope();
    body->codegen(codegenvis);
    codegenvis.popScope();

    if (Bldr->GetInsertBlock()->getTerminator() == nullptr) {
        Bldr->CreateBr(mergeBB);
    }

    thenBB = Bldr->GetInsertBlock();

    if (elseStmt != nullptr) {
        func->insert(func->end(), elseBB);
        Bldr->SetInsertPoint(elseBB);

        elseStmt->codegen(codegenvis);

        if (Bldr->GetInsertBlock()->getTerminator() == nullptr) {
            Bldr->CreateBr(mergeBB);
        }
    }

    elseBB = Bldr->GetInsertBlock();

    func->insert(func->end(), mergeBB);
    Bldr->SetInsertPoint(mergeBB);

    llvm::verifyFunction(*func);
}

ElseStmt::ElseStmt(std::unique_ptr<Statement> elsebody) {
    body = std::move(elsebody);

    Statement *statmt = body.get();
    line = statmt->line;
    column = statmt->column;
}

void ElseStmt::accept(Visitor &visitor) { visitor.visitElseStmt(*this); }

void ElseStmt::codegen(CodegenVis &codegenvis) {
    codegenvis.pushScope();
    body->codegen(codegenvis);
    codegenvis.popScope();
}

WhileStmt::WhileStmt(std::unique_ptr<Expression> condn,
                     std::unique_ptr<Statement> whilebody) {
    condition = std::move(condn);
    body = std::move(whilebody);

    Expression *expr = condition.get();
    line = expr->line;
    column = expr->column;
}

void WhileStmt::accept(Visitor &visitor) { visitor.visitWhileStmt(*this); }

void WhileStmt::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();

    llvm::Function *func = Bldr->GetInsertBlock()->getParent();

    llvm::BasicBlock *condBB = llvm::BasicBlock::Create(*Cxt, "cond", func);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*Cxt, "whilebody");
    llvm::BasicBlock *afterBB = llvm::BasicBlock::Create(*Cxt, "after");

    codegenvis.loopStack.push(std::make_pair(afterBB, condBB));

    Bldr->CreateBr(condBB);

    Bldr->SetInsertPoint(condBB);

    llvm::Value *cond = condition->codegen(codegenvis);

    if (!cond) {
        return;
    }

    llvm::Value *zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Cxt), 0);
    cond = Bldr->CreateICmpNE(cond, zero, "whilecond");

    Bldr->CreateCondBr(cond, bodyBB, afterBB);

    func->insert(func->end(), bodyBB);
    Bldr->SetInsertPoint(bodyBB);

    codegenvis.pushScope();
    body->codegen(codegenvis);
    codegenvis.popScope();

    bodyBB = Bldr->GetInsertBlock();

    if (Bldr->GetInsertBlock()->getTerminator() == nullptr) {
        Bldr->CreateBr(condBB);
    }

    func->insert(func->end(), afterBB);
    Bldr->SetInsertPoint(afterBB);

    codegenvis.loopStack.pop();
    llvm::verifyFunction(*func);
}

BreakStmt::BreakStmt(int tline, int tcol) {
    line = tline;
    column = tcol;
}

void BreakStmt::accept(Visitor &visitor) { visitor.visitBreakStmt(*this); }

void BreakStmt::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();

    if (!codegenvis.loopStack.empty()) {
        auto [afterBB, condBB] = codegenvis.loopStack.top();
        Bldr->CreateBr(afterBB);
    }
}

bool BreakStmt::isTerminator() { return true; }

ContinueStmt::ContinueStmt(int tline, int tcol) {
    line = tline;
    column = tcol;
}

void ContinueStmt::accept(Visitor &visitor) {
    visitor.visitContinueStmt(*this);
}

void ContinueStmt::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();

    if (!codegenvis.loopStack.empty()) {
        auto [afterBB, condBB] = codegenvis.loopStack.top();
        Bldr->CreateBr(condBB);
    }
}

bool ContinueStmt::isTerminator() { return true; }

ReturnStmt::ReturnStmt(std::unique_ptr<Expression> retexpr) {
    retExpr = std::move(retexpr);

    Expression *expr = retExpr.get();
    line = expr->line;
    column = expr->column;
}

void ReturnStmt::accept(Visitor &visitor) { visitor.visitReturnStmt(*this); }

void ReturnStmt::codegen(CodegenVis &codegenvis) {
    llvm::Value *retVal = retExpr->codegen(codegenvis);
    codegenvis.Builder->CreateRet(retVal);
}

bool ReturnStmt::isTerminator() { return true; }

DeclStmt::DeclStmt(TypeKind *tk, std::string varname,
                   std::unique_ptr<Expression> expr, int tline, int tcol) {
    type = tk;
    name = varname;
    expression = std::move(expr);
    line = tline;
    column = tcol;
}

void DeclStmt::accept(Visitor &visitor) { visitor.visitDeclStmt(*this); }

void DeclStmt::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::Function *func = Bldr->GetInsertBlock()->getParent();

    llvm::AllocaInst *alloca =
        codegenvis.CreateEntryBlockAlloca(func, name, type);

    if (expression != nullptr) {
        llvm::Value *initVal = expression->codegen(codegenvis);

        Bldr->CreateStore(initVal, alloca);
    }

    codegenvis.insertName(name, alloca);
}

StructField::StructField(TypeKind *tk, std::string fieldName, int tline,
                         int tcol) {
    type = tk;
    fName = fieldName;
    line = tline;
    column = tcol;
}

void StructField::accept(Visitor &visitor) { visitor.visitStructField(*this); }

StructDecl::StructDecl(std::string tagName, int tline, int tcol) {
    tag = tagName;
    line = tline;
    column = tcol;
}

void StructDecl::addField(std::unique_ptr<StructField> field) {
    fields.push_back(std::move(field));
}

void StructDecl::accept(Visitor &visitor) { visitor.visitStructDecl(*this); }

void StructDecl::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();

    std::vector<llvm::Type *> types;

    for (size_t i = 0; i < fields.size(); i++) {
        types.push_back(codegenvis.tkToType(fields[i]->type));
    }

    llvm::StructType::create(*Cxt, types, tag);
}
