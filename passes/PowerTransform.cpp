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

bool isPowOf2(int x) {
    double lg = log2(x);
    double decimal = lg - floor(lg);

    return decimal == 0.0;
}

using namespace llvm;

namespace {

    class MyPass : public PassInfoMixin<MyPass> {
        public:
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
           
            IRBuilder<> Builder = IRBuilder<>(F.getContext()); 

            for (BasicBlock& BB: F) {
                for (auto it = BB.begin(); it != BB.end();) {
                    Instruction &I = *it++;
                    Builder.SetInsertPoint(&I);

                    const char* op = I.getOpcodeName();

                    if (std::string_view(op) == "mul") {
                        Value* op1 = I.getOperand(0);
                        Value* op2 = I.getOperand(1);
                        
                        ConstantInt* cst1 = dyn_cast<ConstantInt>(op1);
                        ConstantInt* cst2 = dyn_cast<ConstantInt>(op2);
                        
                        auto transform = [&I, op, &Builder](ConstantInt* cst, Value* other) {
                            if (cst) {
                                int val = cst->getSExtValue();

                                if (isPowOf2(val)) {
                                    Value* value = Builder.CreateShl(other, log2(val), "shl");

                                    I.replaceAllUsesWith(value);
                                    I.eraseFromParent();
                                } 
                            }
                        };
                        
                        transform(cst1, op2);
                        transform(cst2, op1);
                    }
                }
            }
            
            F.print(errs());
            return PreservedAnalyses::none();
        }
    };
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {

  return {
    LLVM_PLUGIN_API_VERSION,
    "PowerTransform",
    "0.1",

    [](PassBuilder &PB) {

      PB.registerPipelineParsingCallback(
        [](StringRef Name,
           FunctionPassManager &FPM,
           ArrayRef<PassBuilder::PipelineElement>) {

          if (Name == "power-transform") {
            FPM.addPass(MyPass());
            return true;
          }

          return false;
        });
    }
  };
}
