#ifndef CODEGENVIS_H
#define CODEGENVIS_H

#include <memory>
#include <map>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Verifier.h"

#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"

#include "llvm/Support/raw_ostream.h"

class CodegenVis {
    public:
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::Module> Module;
    std::map<std::string, llvm::AllocaInst*> NamedValues;

    void initModule();
    llvm::Value* LogErrorV(const char *Str);
};

#endif
