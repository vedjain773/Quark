#include "passes/PowerPass.hpp"
#include "llvm/IR/CFG.h"
#include <algorithm>

using namespace llvm;

PreservedAnalyses Mem2Reg::run(Function &F, FunctionAnalysisManager &) {

    IRBuilder<> Builder = IRBuilder<>(F.getContext()); 

    for (BasicBlock& BB: F)
        blockList.insert(&BB);  

    initDomSets();

    while(!runIteration());

    for (BasicBlock* BB: blockList)
        iDoms[BB] = getIDom(BB);

    buildDomTree();

    getDomFrontiers();    

    return PreservedAnalyses::none();
}

void Mem2Reg::initDomSets() {
    for (BasicBlock* BB: blockList) {
        if (BB->isEntryBlock()) {
            BlockSet selfContList = {BB};
            domSets[BB] = selfContList;
        } else {
            BlockSet allNodeList = blockList;
            domSets[BB] = allNodeList;
        }
    } 
}

BlockSet Mem2Reg::getIntersection(BlockSet bs1, BlockSet bs2) {
    BlockSet result;

    std::set_intersection(
        bs1.begin(), bs1.end(),
        bs2.begin(), bs2.end(),
        std::inserter(result, result.begin())
    );

    return result;
}

bool Mem2Reg::runIteration() {
    int changes = 0;

    for (BasicBlock* BB: blockList) {
        if (BB->isEntryBlock())
            continue;

        BlockSet initialList = blockList;

        for (BasicBlock* Pred: predecessors(*BB)) {
            BlockSet predDomSet = domSets[Pred];

            initialList = getIntersection(initialList, predDomSet);
        }

        initialList.insert(BB);

        if (domSets[BB] != initialList) {
            changes++;
            domSets[BB] = initialList;
        }
    }
        
    return changes == 0;
}

BasicBlock* Mem2Reg::getIDom(BasicBlock* BB) {
    BlockSet strictDomSet = domSets[BB];
    strictDomSet.erase(BB);

    std::vector<BasicBlock*> strictDomVec(strictDomSet.begin(), strictDomSet.end());

    for (int i = 0; i < strictDomVec.size(); i++) {

        bool allBlocksFound = true;
        BlockSet currBlockList = domSets[strictDomVec[i]];
        
        for (int j = 0; j < strictDomVec.size(); j++) {
           
            if (i == j) continue;

            if (currBlockList.count(strictDomVec[i]) == 0)
                allBlocksFound = false;
        }

        if (allBlocksFound)
            return strictDomVec[i];
    }

    return nullptr;
}

void Mem2Reg::buildDomTree() {
    for (BasicBlock* BB: blockList) {
        BlockSet valSet;

        for (BasicBlock* BBInner: blockList) {
            if (iDoms[BBInner] == BB)
                valSet.insert(BBInner);
        }

        domTree[BB] = valSet;
    }
}

void Mem2Reg::getDomFrontiers() {
    for (BasicBlock* BB: blockList) {
        if (BB->isEntryBlock() || !BB->hasNPredecessorsOrMore(2)) 
            continue;

        for (BasicBlock* Pred: predecessors(*BB)) {
            BasicBlock* currentNode = Pred;

            while (currentNode != iDoms[BB]) {
                domFrontier[currentNode].insert(BB);
                currentNode = iDoms[currentNode];
            }
        }
    }
}
