#include "passes/PowerTransform.hpp"

using namespace llvm;

bool isPowOf2(int x) {
    double lg = log2(x);
    double decimal = lg - floor(lg);

    return decimal == 0.0;
}

PreservedAnalyses MyPass::run(Function &F, FunctionAnalysisManager &) {

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
