#ifndef EXTERNALDECL_H
#define EXTERNALDECL_H

#include "visitors/CodegenVis.hpp"
#include "visitors/Visitor.hpp"

class ExternalDecl {
  public:
    virtual void accept(Visitor &visitor) = 0;
    virtual void codegen(CodegenVis &codegenvis) = 0;
    virtual ~ExternalDecl() = default;
};

#endif
