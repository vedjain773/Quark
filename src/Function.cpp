#include "Function.hpp"
#include <cstddef>
#include <iostream>

using size_t = std::size_t;

Parameter::Parameter(TokenType p_type, std::string p_name)
    : type(TokToType(p_type)), name(p_name) {}

Parameter::Parameter(TypeKind *p_type, std::string p_name)
    : type(p_type), name(p_name) {}

void Parameter::accept(Visitor &visitor) { visitor.visitParameter(*this); }

llvm::Value *Parameter::codegen(CodegenVis &codegenvis) {
    // do nothing
}

Prototype::Prototype(TokenType ret_type, std::string func_name, int tline,
                     int tcol)
    : retType(TokToType(ret_type)), funcName(func_name), line(tline),
      column(tcol) {}

Prototype::Prototype(TypeKind *ret_type, std::string func_name, int tline,
                     int tcol)
    : retType(ret_type), funcName(func_name), line(tline), column(tcol) {}

void Prototype::addParam(std::unique_ptr<Parameter> param) {
    paramList.push_back(std::move(param));
}

llvm::Function *Prototype::codegen(CodegenVis &codegenvis) {
    llvm::Module *Mod = (codegenvis.Module).get();
    std::vector<llvm::Type *> typeVec;

    for (size_t i = 0; i < paramList.size(); i++) {
        Parameter *param = (paramList[i]).get();
        typeVec.push_back(codegenvis.tkToType(param->type));
    }

    llvm::FunctionType *functype =
        llvm::FunctionType::get(codegenvis.tkToType(retType), typeVec, false);
    llvm::Function *func = llvm::Function::Create(
        functype, llvm::Function::ExternalLinkage, funcName, Mod);

    unsigned Idx = 0;
    for (auto &Arg : func->args()) {
        Parameter *param = (paramList[Idx++]).get();
        Arg.setName(param->name);
    }

    return func;
}

void Prototype::accept(Visitor &visitor) { visitor.visitPrototype(*this); }

FuncDef::FuncDef(std::unique_ptr<Prototype> proto_type,
                 std::unique_ptr<BlockStmt> func_body)
    : prototype(std::move(proto_type)), funcBody(std::move(func_body)) {}

void FuncDef::accept(Visitor &visitor) { visitor.visitFuncDef(*this); }

void FuncDef::codegen(CodegenVis &codegenvis) {
    llvm::LLVMContext *Cxt = (codegenvis.Context).get();
    llvm::Module *Mod = (codegenvis.Module).get();
    llvm::IRBuilder<> *Bldr = (codegenvis.Builder).get();

    llvm::Function *func = Mod->getFunction(prototype->funcName);

    if (!func)
        func = prototype->codegen(codegenvis);

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*Cxt, "entry", func);
    Bldr->SetInsertPoint(BB);

    codegenvis.pushScope();

    size_t i = 0;
    Prototype *proto = prototype.get();
    for (auto &Arg : func->args()) {
        Parameter *param = (proto->paramList[i++]).get();

        std::string paramAllocaStr = Arg.getName().str() + ".addr";

        llvm::AllocaInst *alloca = codegenvis.CreateEntryBlockAlloca(
            func, paramAllocaStr, param->type);

        Bldr->CreateStore(&Arg, alloca);

        codegenvis.insertName(Arg.getName().str(), alloca);
    }

    funcBody->codegen(codegenvis);
    codegenvis.popScope();

    if (prototype->retType == getType("void")) {
        llvm::BasicBlock &lastBlock = func->back();
        if (lastBlock.getTerminator() == nullptr)
            Bldr->CreateRetVoid();
    }

    llvm::verifyFunction(*func);
}
