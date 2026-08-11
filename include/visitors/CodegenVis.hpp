#ifndef CODEGENVIS_H
#define CODEGENVIS_H

#include "utils/Scope.hpp"
#include <map>
#include <memory>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Verifier.h"

#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"

#include "llvm/Support/raw_ostream.h"
#include "llvm/TargetParser/Triple.h"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/CodeGen.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"

enum class Operators;

class CodegenVis {
  public:
    std::unique_ptr<llvm::LLVMContext> Context;
    std::unique_ptr<llvm::IRBuilder<>> Builder;
    std::unique_ptr<llvm::Module> Module;
    std::vector<std::map<std::string, llvm::AllocaInst *>> scopes;
    std::stack<std::pair<llvm::BasicBlock *, llvm::BasicBlock *>> loopStack;

    void initModule(std::string fileName);
    llvm::Value *LogErrorV(std::string errMsg);
    llvm::Type *tkToType(TypeKind *typek);
    llvm::AllocaInst *CreateEntryBlockAlloca(llvm::Function *function,
                                             std::string varname, TypeKind *tk);

    llvm::Value *handlePointerArithmetic(llvm::Value *left, llvm::Value *right,
                                         TypeKind *typek, Operators Op);
    llvm::Value *handleBinOp(llvm::Value *left, llvm::Value *right,
                             Operators Op, TypeKind *infType);

    void pushScope();
    void popScope();
    void insertName(std::string name, llvm::AllocaInst *alloca);
    llvm::AllocaInst *lookup(std::string name);

    void emitObj(std::string Filename);
};

#endif
