#include "CodegenVis.hpp"
#include <iostream>

void CodegenVis::initModule() {
    Context = std::make_unique<llvm::LLVMContext>();
    Module = std::make_unique<llvm::Module>("Moddex", *Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
}

llvm::Value* CodegenVis::LogErrorV(std::string errMsg) {
    std::cout << errMsg << "\n";
    return nullptr;
}

llvm::Type* CodegenVis::tkToType(TypeKind tk) {
    switch (tk) {
        case TypeKind::INT: {
            return llvm::Type::getInt32Ty(*Context);
        }
        break;

        case TypeKind::CHAR: {
            return llvm::Type::getInt8Ty(*Context);
        }
        break;

        case TypeKind::VOID: {
            return llvm::Type::getVoidTy(*Context);
        }
        break;
    }
}

llvm::AllocaInst* CodegenVis::CreateEntryBlockAlloca(llvm::Function* function, std::string varname, TypeKind tk) {
    llvm::StringRef VarName(varname);
    llvm::IRBuilder<> TmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return TmpB.CreateAlloca(tkToType(tk), nullptr, VarName);
}

void CodegenVis::pushScope() {
    scopes.emplace_back();
}

void CodegenVis::popScope() {
    assert(!scopes.empty() && "Popping empty scope!");
    scopes.pop_back();
}

void CodegenVis::insertName(std::string name, llvm::AllocaInst* alloca) {
    scopes[scopes.size() - 1].insert({name, alloca});
}

llvm::AllocaInst* CodegenVis::lookup(std::string name) {
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].count(name)) {
            return scopes[i][name];
        }
    }

    return nullptr;
}
