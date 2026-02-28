#ifndef PROGRAM_H
#define PROGRAM_H

#include "ExternalDecl.hpp"
#include "Visitor.hpp"
#include "Statement.hpp"
#include <memory>
#include <vector>
#include <string>

class Program {
    public:
    CodegenVis codegenvis;
    std::vector<std::unique_ptr<ExternalDecl>> root;

    void accept(Visitor& visitor);
    void add(std::unique_ptr<ExternalDecl> edecl);
    void printAST();
    void semAnalyse();
    void codegen();
    void emitObj(std::string fileName);
    void printIR();
};

#endif
