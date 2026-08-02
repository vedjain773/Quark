#include "Optimizer.hpp"
#include "passes/ConstantProp.hpp"
#include "passes/DeadBranchPass.hpp"
#include "passes/DeadInstPass.hpp"
#include "passes/IdentityPass.hpp"
#include "passes/Mem2Reg.hpp"
#include "passes/PowerPass.hpp"
#include "llvm/IR/Verifier.h"

using namespace llvm;

void Optimizer::registerPasses() {
    FPM = std::make_unique<FunctionPassManager>();
    LAM = std::make_unique<LoopAnalysisManager>();
    FAM = std::make_unique<FunctionAnalysisManager>();
    MAM = std::make_unique<ModuleAnalysisManager>();
    CGAM = std::make_unique<CGSCCAnalysisManager>();

    FPM->addPass(Mem2Reg());

    for (int i = 0; i < 3; i++) {
        FPM->addPass(IdentityPass());
        FPM->addPass(PowerPass());
        FPM->addPass(DeadBranchPass());
        FPM->addPass(DeadInstPass());
        FPM->addPass(ConstantPropPass());
    }

    PB.registerModuleAnalyses(*MAM);
    PB.registerFunctionAnalyses(*FAM);
    PB.crossRegisterProxies(*LAM, *FAM, *CGAM, *MAM);
}

void Optimizer::run(Module &mod) {
    for (Function &F : mod) {
        FPM->run(F, *FAM);
        verifyFunction(F, &errs());
    }
}
