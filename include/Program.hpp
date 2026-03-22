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
    std::vector<std::unique_ptr<ExternalDecl>> root;
    std::string fileName;
    CodegenVis codegenvis;

    void setFileName(std::string file_name);
    std::unique_ptr<Program> optimize(OptimizeVisitor& optvis);
    void accept(Visitor& visitor);
    void add(std::unique_ptr<ExternalDecl> edecl);

    void printAST();
    int semAnalyse();
    std::unique_ptr<Program> optimize();
    void codegen();
    void printIR();

    void emitObj(std::string fileName);
};

#endif
