#ifndef EXTERNALDECL_H
#define EXTERNALDECL_H

#include "Visitor.hpp"
#include "CodegenVis.hpp"

class ExternalDecl {
    public:
    virtual ~ExternalDecl() = default;
    virtual void accept(Visitor& visitor) = 0;
    virtual llvm::Value* codegen(CodegenVis& codegenvis) = 0;
};

#endif
