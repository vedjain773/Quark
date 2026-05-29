#include "passes/PowerPass.hpp"
#include "passes/IdentityPass.hpp"

using namespace llvm;

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {

    return {
        LLVM_PLUGIN_API_VERSION,
        "Alg",
        "0.1",

        [](PassBuilder &PB) {

        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {

                if (Name == "alg") {
                    FPM.addPass(IdentityPass());
                    FPM.addPass(PowerPass());
                    return true;
                }

                return false;
            });
        }
    };
}
