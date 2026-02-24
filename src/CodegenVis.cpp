#include "CodegenVis.hpp"

void CodegenVis::initModule() {
    Context = std::make_unique<llvm::LLVMContext>();
    Module = std::make_unique<llvm::Module>("Moddex", *Context);
    Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
}

llvm::Value* CodegenVis::LogErrorV(const char *Str) {
    return nullptr;
}
