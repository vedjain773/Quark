#ifndef FUNCTION_H
#define FUNCTION_H

#include "frontend/scanner/Token.hpp"
#include "nodes/ExternalDecl.hpp"
#include "nodes/Statement.hpp"
#include "utils/Scope.hpp"
#include "visitors/Visitor.hpp"

class Parameter {
  public:
    TypeKind *type;
    std::string name;

    Parameter(TypeKind *p_type, const std::string &p_name);

    void accept(Visitor &visitor);
};

class Prototype {
  public:
    TypeKind *retType;
    std::string funcName;
    int line;
    int column;
    std::vector<std::unique_ptr<Parameter>> paramList;

    Prototype(TypeKind *ret_type, const std::string &func_name, int line, int column);

    void addParam(std::unique_ptr<Parameter> param);
    void accept(Visitor &visitor);
    llvm::Function *codegen(CodegenVis &codegenvis);
};

class FuncDef : public ExternalDecl {
  public:
    std::unique_ptr<Prototype> prototype;
    std::unique_ptr<BlockStmt> funcBody;

    FuncDef(std::unique_ptr<Prototype> proto_type, std::unique_ptr<BlockStmt> func_body);

    void accept(Visitor &visitor);
    void codegen(CodegenVis &codegenvis);
};

#endif
