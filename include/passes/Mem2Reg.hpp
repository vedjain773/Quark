#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string_view>
#include <vector>

namespace llvm {
using BlockVec = std::vector<BasicBlock *>;
using BlockSet = std::set<BasicBlock *>;
using ValSet = std::set<Value *>;
using domMap = std::map<BasicBlock *, BlockSet>;

class Mem2Reg : public PassInfoMixin<Mem2Reg> {
private:
  BlockSet blockList;
  domMap domSets;
  std::map<BasicBlock *, BasicBlock *> iDoms;
  domMap domTree;
  domMap domFrontier;
  domMap iDF;

  BlockVec blockVecList;
  std::map<Value *, std::set<PHINode *>> valPhiPos;
  std::map<Value *, std::stack<Value *>> allocaValStack;
  std::map<Value *, int> counter;

  //Live analysis
  std::map<BasicBlock *, ValSet> UseMap;
  std::map<BasicBlock *, ValSet> DefMap;
  std::map<BasicBlock *, ValSet> LiveInMap;
  std::map<BasicBlock *, ValSet> LiveOutMap;
  
  void performLiveAnalysis();
  
  bool isEntryBlock(BasicBlock* BB);

  void initDomSets();
  
  ValSet getDiff(ValSet vs1, ValSet vs2);
  ValSet getUnion(ValSet vs1, ValSet vs2);
  BlockSet getIntersection(BlockSet bs1, BlockSet bs2);
  bool runIteration();

  BasicBlock *getIDom(BasicBlock *BB);
  void buildDomTree();

  void getDomFrontiers();

  BlockSet computeIDF(BlockVec defSites);
  BlockVec getDefSites(AllocaInst *allocainst);
  std::map<BasicBlock *, StoreInst *> getBlockDefs(AllocaInst *allocainst);
  void PlacePHINodes();

  bool isPredOf(BasicBlock *child, BasicBlock *Parent);

  void renamePass();
  
  std::set<Value*> promotableAllocas;
  void getPromAllocas();
  
  std::string getNewName(Value *allocainst);
  void rename(BasicBlock *);
  
  void reset();
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &);
};
}; // namespace llvm
