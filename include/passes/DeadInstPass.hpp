#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string_view>

namespace llvm {
class DeadInstPass : public PassInfoMixin<DeadInstPass> {
  public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};
}; // namespace llvm
