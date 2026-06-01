#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string_view>
#include <vector>
#include <map>
#include <set>

namespace llvm {
    using BlockSet std::set<BasicBlock*>;
    using domMap std::map<BasicBlock*, BlockSet>;

    class Mem2Reg: public PassInfoMixin<Mem2Reg> {
        private:
        BlockSet blockList;
        domMap domSets;
        std::map<BasicBlock*, BasicBlock*> iDoms;
        domMap domTree;
        domMap domFrontier;

        void initDomSets();

        Blockset getIntersection(BlockSet bs1, BlockSet bs2);
        bool runIteration();
        
        BasicBlock* getIDom(BasicBlock* BB);
        void buildDomTree();
        
        void getDomFrontiers();

        public:
        PreservedAnalyses run(Function &F, FunctionAnalysisManager &); 
    }; 
};

