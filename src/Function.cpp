#include "Function.hpp"
#include <iostream>

Parameter::Parameter(TokenType p_type, std::string p_name) {
    type = TokToType(p_type);
    name = p_name;
}

void Parameter::accept(Visitor& visitor) {
    visitor.visitParameter(*this);
}

llvm::Value* Parameter::codegen(CodegenVis& codegenvis) {

}

Prototype::Prototype(TokenType ret_type, std::string func_name, int tline, int tcol) {
    retType = TokToType(ret_type);
    funcName = func_name;
    line = tline;
    column = tcol;
}

void Prototype::addParam(std::unique_ptr<Parameter> param) {
    paramList.push_back(std::move(param));
}

llvm::Function* Prototype::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    llvm::Module* Mod = (codegenvis.Module).get();
    std::vector<llvm::Type*> typeVec;

    llvm::FunctionType *FT = llvm::FunctionType::get(codegenvis.tkToType(retType), typeVec, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, funcName, Mod);

    return F;
}

void Prototype::accept(Visitor& visitor) {
    visitor.visitPrototype(*this);
}

FuncDef::FuncDef(std::unique_ptr<Prototype> proto_type, std::unique_ptr<BlockStmt> func_body) {
    prototype = std::move(proto_type);
    funcBody = std::move(func_body);
}

void FuncDef::accept(Visitor& visitor) {
    visitor.visitFuncDef(*this);
}

llvm::Value* FuncDef::codegen(CodegenVis& codegenvis) {
    llvm::LLVMContext* Cxt = (codegenvis.Context).get();
    llvm::Module* Mod = (codegenvis.Module).get();
    llvm::IRBuilder<>* Bldr = (codegenvis.Builder).get();

    llvm::Function *func = Mod->getFunction(prototype->funcName);

    if (!func)
    func = prototype->codegen(codegenvis);

    if (!func)
    return nullptr;

    llvm::BasicBlock* BB = llvm::BasicBlock::Create(*Cxt, "entry", func);
    Bldr->SetInsertPoint(BB);

    codegenvis.pushScope();
    funcBody->codegen(codegenvis);
    codegenvis.popScope();

    llvm::verifyFunction(*func);

    return func;
}
