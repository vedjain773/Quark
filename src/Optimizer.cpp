#include "passes/PowerPass.hpp"
#include "passes/IdentityPass.hpp"
#include "passes/DeadInstPass.hpp"
#include "llvm/Transforms/Utils/Mem2Reg.h"
#include "Optimizer.hpp"

using namespace llvm;

void Optimizer::registerPasses() {
    FPM = std::make_unique<FunctionPassManager>();
    LAM = std::make_unique<LoopAnalysisManager>();
    FAM = std::make_unique<FunctionAnalysisManager>();
    MAM = std::make_unique<ModuleAnalysisManager>();
    CGAM = std::make_unique<CGSCCAnalysisManager>();

    FPM->addPass(IdentityPass());
    FPM->addPass(PowerPass());
    FPM->addPass(DeadInstPass());
    FPM->addPass(PromotePass());

    PB.registerModuleAnalyses(*MAM);
    PB.registerFunctionAnalyses(*FAM);
    PB.crossRegisterProxies(*LAM, *FAM, *CGAM, *MAM);
}

void Optimizer::run(Module &mod) {
    for (Function& F: mod) {
        FPM->run(F, *FAM);
    }
}
