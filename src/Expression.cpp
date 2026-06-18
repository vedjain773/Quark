#include "Expression.hpp"
#include <iostream>
#include <map>

std::map<Operators, std::string> enumToStr = {
    {Operators::BANG, "!"},    {Operators::MINUS, "-"},
    {Operators::MODULUS, "%"}, {Operators::DIVIDE, "/"},
    {Operators::MULT, "*"},    {Operators::PLUS, "+"},
    {Operators::GREATER, ">"}, {Operators::GREATER_EQUALS, ">="},
    {Operators::LESS, "<"},    {Operators::LESS_EQUALS, "<="},
    {Operators::EQUALS, "=="}, {Operators::NOT_EQUALS, "!="},
    {Operators::AND, "&&"},    {Operators::OR, "||"},
};

std::map<std::string, Operators> strToEnum = {
    {"!", Operators::BANG},
    {"-", Operators::MINUS},
    {"%", Operators::MODULUS},
    {"/", Operators::DIVIDE},
    {"*", Operators::MULT},
    {"+", Operators::PLUS},
    {">", Operators::GREATER},
    {">=", Operators::GREATER_EQUALS},
    {
        "<",
        Operators::LESS,
    },
    {"<=", Operators::LESS_EQUALS},
    {"==", Operators::EQUALS},
    {"!=", Operators::NOT_EQUALS},
    {"&&", Operators::AND},
    {"||", Operators::OR},
};

std::string getOpStr(Operators op) { return enumToStr[op]; }

Operators getOp(std::string opStr) { return strToEnum[opStr]; }

IntExpr::IntExpr(int value, int tline, int tcol) {
    Val = value;
    line = tline;
    column = tcol;
}

void IntExpr::accept(Visitor &visitor) { visitor.visitIntExpr(*this); }

llvm::Value *IntExpr::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();
    return llvm::ConstantInt::get(*Cxt, llvm::APInt(32, Val, true));
}

CharExpr::CharExpr(char charac, int tline, int tcol) {
    character = charac;
    line = tline;
    column = tcol;
}

void CharExpr::accept(Visitor &visitor) { visitor.visitCharExpr(*this); }

llvm::Value *CharExpr::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();
    return llvm::ConstantInt::get(*Cxt, llvm::APInt(8, character));
}

VarExpr::VarExpr(std::string name, int tline, int tcol) {
    Name = name;
    line = tline;
    column = tcol;
}

void VarExpr::accept(Visitor &visitor) { visitor.visitVarExpr(*this); }

llvm::Value *VarExpr::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::AllocaInst *alloca = codegenvis.lookup(Name);

    if (!alloca) {
        return codegenvis.LogErrorV("Use of undeclared variable: " + Name);
    }

    return Bldr->CreateLoad(alloca->getAllocatedType(), alloca, Name.c_str());
}

llvm::Value *VarExpr::emitPtr(CodegenVis &codegenvis) {
    llvm::AllocaInst *alloca = codegenvis.lookup(Name);

    if (!alloca) {
        return codegenvis.LogErrorV("Use of undeclared variable: " + Name);
    }

    return alloca;
}

bool VarExpr::isLValue() { return true; }

DerefExpr::DerefExpr(std::unique_ptr<Expression> expression, int tline,
                     int tcol) {
    expr = std::move(expression);
    line = tline;
    column = tcol;
}

void DerefExpr::accept(Visitor &visitor) { visitor.visitDerefExpr(*this); }

llvm::Value *DerefExpr::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();

    llvm::Value *ptr = expr->codegen(codegenvis);

    llvm::Type *type = nullptr;

    TypeKind *tk = expr->infType->to;
    type = codegenvis.tkToType(tk);

    return Bldr->CreateLoad(type, ptr);
}

llvm::Value *DerefExpr::emitPtr(CodegenVis &codegenvis) {
    return expr->codegen(codegenvis);
}

bool DerefExpr::isLValue() { return true; }

AddressExpr::AddressExpr(std::unique_ptr<Expression> expression, int tline,
                         int tcol) {
    expr = std::move(expression);
    line = tline;
    column = tcol;
}

void AddressExpr::accept(Visitor &visitor) { visitor.visitAddressExpr(*this); }

llvm::Value *AddressExpr::codegen(CodegenVis &codegenvis) {
    return expr->emitPtr(codegenvis);
}

CastExpr::CastExpr(std::unique_ptr<Expression> expression, TypeKind *from_tk,
                   TypeKind *to_tk) {
    expr = std::move(expression);
    from = from_tk;
    to = to_tk;
}

void CastExpr::accept(Visitor &visitor) { visitor.visitCastExpr(*this); }

llvm::Value *CastExpr::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::Value *val = expr->codegen(codegenvis);

    if (from == getType("char") && to == getType("int")) {
        return Bldr->CreateZExt(val, codegenvis.tkToType(to), "castext");
    } else if (from == getType("int") && to == getType("char")) {
        return Bldr->CreateTrunc(val, codegenvis.tkToType(to), "casttrunc");
    } else {
        return nullptr;
    }
}

UnaryExpr::UnaryExpr(Operators op, std::unique_ptr<Expression> operand,
                     int tline, int tcol) {
    Op = op;
    Operand = std::move(operand);
    line = tline;
    column = tcol;
}

void UnaryExpr::accept(Visitor &visitor) { visitor.visitUnaryExpr(*this); }

llvm::Value *UnaryExpr::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::Value *val = Operand->codegen(codegenvis);

    switch (Op) {
    case Operators::MINUS: {
        return Bldr->CreateNeg(val);
    } break;

    case Operators::BANG: {
        llvm::Value *zero =
            llvm::ConstantInt::get(llvm::Type::getInt32Ty(*Cxt), 0);

        llvm::Value *cmp = Bldr->CreateICmpNE(val, zero, "compne");

        llvm::Value *exor = Bldr->CreateXor(cmp, true, "xor");

        return Bldr->CreateZExt(exor, llvm::Type::getInt32Ty(*Cxt));
    } break;

    default: {
        return val;
    }
    }
}

AssignExpr::AssignExpr(std::unique_ptr<Expression> lhs,
                       std::unique_ptr<Expression> rhs, int tline, int tcol) {
    LHS = std::move(lhs);
    RHS = std::move(rhs);
    line = tline;
    column = tcol;
}

void AssignExpr::accept(Visitor &visitor) { visitor.visitAssignExpr(*this); }

llvm::Value *AssignExpr::codegen(CodegenVis &codegenvis) {
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();

    llvm::Value *addr = LHS->emitPtr(codegenvis);

    llvm::Value *exprVal = RHS->codegen(codegenvis);
    Bldr->CreateStore(exprVal, addr);
    return exprVal;
}

void EmptyExpr::accept(Visitor &visitor) { visitor.visitEmptyExpr(*this); }

llvm::Value *EmptyExpr::codegen(CodegenVis &codegenvis) { return nullptr; }

CallExpr::CallExpr(std::string callee_name, int tline, int tcol) {
    callee = callee_name;
    line = tline;
    column = tcol;
}

void CallExpr::add(std::unique_ptr<Expression> arg) {
    args.push_back(std::move(arg));
}

void CallExpr::accept(Visitor &visitor) { visitor.visitCallExpr(*this); }

llvm::Value *CallExpr::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Context = (codegenvis.Context).get();
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();
    llvm::Module *module = (codegenvis.Module).get();
    llvm::Function *calleefunc = module->getFunction(callee);

    if (!calleefunc)
        return codegenvis.LogErrorV("Unknown function referenced");

    if (calleefunc->arg_size() != args.size())
        return codegenvis.LogErrorV("Incorrect no. of arguments passed");

    std::vector<llvm::Value *> ArgsV;

    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen(codegenvis));
        if (!ArgsV.back())
            return nullptr;
    }

    if (calleefunc->getReturnType() != llvm::Type::getVoidTy(*Context))
        return Bldr->CreateCall(calleefunc, ArgsV, "calltmp");
    else
        return Bldr->CreateCall(calleefunc, ArgsV);
}

BinaryExpr::BinaryExpr(Operators op, std::unique_ptr<Expression> lhs,
                       std::unique_ptr<Expression> rhs, int tline, int tcol) {
    LHS = std::move(lhs);
    RHS = std::move(rhs);
    Op = op;
    line = tline;
    column = tcol;
}

void BinaryExpr::accept(Visitor &visitor) { visitor.visitBinaryExpr(*this); }

llvm::Value *BinaryExpr::codegen(CodegenVis &codegenvis) {
    llvm::Value *left = LHS->codegen(codegenvis);
    llvm::Value *right = RHS->codegen(codegenvis);

    if (!LHS || !RHS) {
        return nullptr;
    }

    if (isPointerType(LHS->infType)) {
        return codegenvis.handlePointerArithmetic(left, right, LHS->infType->to,
                                                  Op);
    }

    return codegenvis.handleBinOp(left, right, Op, infType);
}
