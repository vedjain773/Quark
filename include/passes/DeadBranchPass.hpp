#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <cmath>
#include <iostream>
#include <string_view>

namespace llvm {
class DeadBranchPass : public PassInfoMixin<DeadBranchPass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};
}; 
