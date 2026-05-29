#include "passes/PowerTransform.hpp"

using namespace llvm;

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {

    return {
        LLVM_PLUGIN_API_VERSION,
        "PowerTransform",
        "0.1",

        [](PassBuilder &PB) {

        PB.registerPipelineParsingCallback(
            [](StringRef Name, FunctionPassManager &FPM, ArrayRef<PassBuilder::PipelineElement>) {

                if (Name == "power-transform") {
                    FPM.addPass(MyPass());
                    return true;
                }

                return false;
            });
        }
    };
}
