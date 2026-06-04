#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class Optimizer {
public:
  PassBuilder PB;
  std::unique_ptr<FunctionPassManager> FPM;
  std::unique_ptr<LoopAnalysisManager> LAM;
  std::unique_ptr<FunctionAnalysisManager> FAM;
  std::unique_ptr<ModuleAnalysisManager> MAM;
  std::unique_ptr<CGSCCAnalysisManager> CGAM;

  void registerPasses();
  void run(Module &mod);
};
