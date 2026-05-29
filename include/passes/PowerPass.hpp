#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string_view>
#include <cmath>

bool isPowOf2(int x);

namespace llvm {
    class PowerPass: public PassInfoMixin<PowerPass> {
        public:
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &); 
    }; 
};

