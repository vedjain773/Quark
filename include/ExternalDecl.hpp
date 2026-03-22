#ifndef EXTERNALDECL_H
#define EXTERNALDECL_H

#include "Visitor.hpp"
#include "Node.hpp"
#include "OptVis.hpp"
#include "CodegenVis.hpp"

class ExternalDecl: public Node {
    public:
    virtual void accept(Visitor& visitor) = 0;
    virtual std::unique_ptr<FuncDef> optimize(OptimizeVisitor& optvis) = 0;
    virtual NodeType getNodeType() = 0;
    virtual llvm::Value* codegen(CodegenVis& codegenvis) = 0;
};

#endif
