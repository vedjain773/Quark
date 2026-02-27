#include "Expression.hpp"
#include <map>
#include <iostream>

std::map<Operators, std::string> enumToStr = {
    {Operators::BANG, "!"},
    {Operators::MINUS, "-"},
    {Operators::MODULUS, "%"},
    {Operators::DIVIDE, "/"},
    {Operators::MULT, "*"},
    {Operators::PLUS, "+"},
    {Operators::GREATER, ">"},
    {Operators::GREATER_EQUALS, ">="},
    {Operators::LESS, "<"},
    {Operators::LESS_EQUALS, "<="},
    {Operators::EQUALS, "=="},
    {Operators::NOT_EQUALS, "!="},
    {Operators::AND, "&&"},
    {Operators::OR, "||"},
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
    {"<", Operators::LESS,},
    {"<=", Operators::LESS_EQUALS},
    {"==", Operators::EQUALS},
    {"!=", Operators::NOT_EQUALS},
    {"&&", Operators::AND},
    {"||", Operators::OR},
};

std::string getOpStr(Operators op) {
    return enumToStr[op];
}

Operators getOp(std::string opStr) {
    return strToEnum[opStr];
}

IntExpr::IntExpr(int value, int tline, int tcol) {
    Val = value;
    line = tline;
    column = tcol;
}

CharExpr::CharExpr(char charac, int tline, int tcol) {
    character = charac;
    line = tline;
    column = tcol;
}

VarExpr::VarExpr(std::string name, int tline, int tcol) {
    Name = name;
    line = tline;
    column = tcol;
}

UnaryExpr::UnaryExpr(Operators op, std::unique_ptr<Expression> operand, int tline, int tcol) {
    Op = op;
    Operand = std::move(operand);
    line = tline;
    column = tcol;
}

BinaryExpr::BinaryExpr(Operators op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, int tline, int tcol) {
    LHS = std::move(lhs);
    RHS = std::move(rhs);
    Op = op;
    line = tline;
    column = tcol;
}

AssignExpr::AssignExpr(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, int tline, int tcol) {
    LHS = std::move(lhs);
    RHS = std::move(rhs);
    line = tline;
    column = tcol;
}

void EmptyExpr::accept(Visitor& visitor) {
    visitor.visitEmptyExpr(*this);
}

CallExpr::CallExpr(std::string callee_name, int tline, int tcol) {
    callee = callee_name;
    line = tline;
    column = tcol;
}

void CallExpr::add(std::unique_ptr<Expression> arg) {
    args.push_back(std::move(arg));
}

void IntExpr::accept(Visitor& visitor) {
    visitor.visitIntExpr(*this);
}

void CharExpr::accept(Visitor& visitor) {
    visitor.visitCharExpr(*this);
}

void VarExpr::accept(Visitor& visitor) {
    visitor.visitVarExpr(*this);
}

void UnaryExpr::accept(Visitor& visitor) {
    visitor.visitUnaryExpr(*this);
}

void BinaryExpr::accept(Visitor& visitor) {
    visitor.visitBinaryExpr(*this);
}

void AssignExpr::accept(Visitor& visitor) {
    visitor.visitAssignExpr(*this);
}

void CallExpr::accept(Visitor& visitor) {
    visitor.visitCallExpr(*this);
}

llvm::Value* IntExpr::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    return llvm::ConstantInt::get(*Cxt, llvm::APInt(32, Val, true));
}

llvm::Value* CharExpr::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    return llvm::ConstantInt::get(*Cxt, llvm::APInt(8, character));
}

llvm::Value* VarExpr::codegen(CodegenVis& codegenvis) {
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();
    llvm::AllocaInst* alloca = codegenvis.lookup(Name);

    if (!alloca) {
        return codegenvis.LogErrorV("Use of undeclared variable: " + Name);
    }

    return Bldr->CreateLoad(alloca->getAllocatedType(), alloca, Name.c_str());
}

llvm::Value* UnaryExpr::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();
    llvm::Value* val = Operand->codegen(codegenvis);

    switch(Op) {
        case Operators::MINUS: {
            return Bldr->CreateNeg(val);
        }
        break;

        case Operators::BANG: {
            llvm::Value* zero =
                llvm::ConstantInt::get(
                    llvm::Type::getInt32Ty(*Cxt), 0
                );

            llvm::Value* cmp = Bldr->CreateICmpNE(val, zero, "compne");

            llvm::Value* exor = Bldr->CreateXor(cmp, true, "xor");

            return Bldr->CreateZExt(exor, llvm::Type::getInt32Ty(*Cxt));
        }
        break;

        default: {
            return val;
        }
    }
}

llvm::Value* BinaryExpr::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();

    llvm::Value* left = LHS->codegen(codegenvis);
    llvm::Value* right = RHS->codegen(codegenvis);

    if (!LHS || !RHS) {
        return nullptr;
    }

    switch (Op) {
        case Operators::PLUS: {
            return Bldr->CreateAdd(left, right, "add", false, true);
        }
        break;

        case Operators::MINUS: {
            return Bldr->CreateSub(left, right, "sub", false, true);
        }
        break;

        case Operators::MULT: {
            return Bldr->CreateMul(left, right, "mul", false, true);
        }
        break;

        case Operators::DIVIDE: {
            return Bldr->CreateSDiv(left, right, "sdiv", false);
        }
        break;

        case Operators::MODULUS: {
            return Bldr->CreateSRem(left, right, "srem");
        }
        break;

        case Operators::GREATER: {
            llvm::Value* gt = Bldr->CreateICmpSGT(left, right, "compSGT");
            return Bldr->CreateZExt(gt, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::GREATER_EQUALS: {
            llvm::Value* ge = Bldr->CreateICmpSGE(left, right, "compSGE");
            return Bldr->CreateZExt(ge, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::LESS: {
            llvm::Value* lt = Bldr->CreateICmpSGT(left, right, "compSLT");
            return Bldr->CreateZExt(lt, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::LESS_EQUALS: {
            llvm::Value* le = Bldr->CreateICmpSLE(left, right, "compSLE");
            return Bldr->CreateZExt(le, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::EQUALS: {
            llvm::Value* ee = Bldr->CreateICmpEQ(left, right, "compEE");
            return Bldr->CreateZExt(ee, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::NOT_EQUALS: {
            llvm::Value* ne = Bldr->CreateICmpNE(left, right, "compNE");
            return Bldr->CreateZExt(ne, codegenvis.tkToType(infType), "ext");
        }
        break;

        case Operators::AND: {
            return left;
        }
        break;

        case Operators::OR: {
            return left;
        }
        break;

        default:
            return left;
    }
}

llvm::Value* EmptyExpr::codegen(CodegenVis& codegenvis) {
    return nullptr;
}

llvm::Value* CallExpr::codegen(CodegenVis& codegenvis) {
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();
    llvm::Module* module = (codegenvis.Module).get();
    llvm::Function* calleefunc = module->getFunction(callee);

    if (!calleefunc)
    return codegenvis.LogErrorV("Unknown function referenced");

    if (calleefunc->arg_size() != args.size())
        return codegenvis.LogErrorV("Incorrect no. of arguments passed");

    std::vector<llvm::Value*> ArgsV;

    for (unsigned i = 0, e = args.size(); i != e; ++i) {
        ArgsV.push_back(args[i]->codegen(codegenvis));
        if (!ArgsV.back())
            return nullptr;
    }

    return Bldr->CreateCall(calleefunc, ArgsV, "calltmp");
}

llvm::Value* AssignExpr::codegen(CodegenVis& codegenvis) {
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();
    VarExpr* lhs = static_cast<VarExpr*>(LHS.get());
    llvm::Value* var = codegenvis.lookup(lhs->Name);

    llvm::Value* exprVal = RHS->codegen(codegenvis);
    Bldr->CreateStore(exprVal, var);
    return exprVal;
}
