#ifndef PROGRAM_H
#define PROGRAM_H

#include "nodes/ExternalDecl.hpp"
#include "nodes/Statement.hpp"
#include "visitors/Visitor.hpp"
#include <memory>
#include <string>
#include <vector>

class Program {
  private:
    std::string fileName;
    CodegenVis codegenvis;

  public:
    std::vector<std::unique_ptr<ExternalDecl>> root;

    void setFileName(std::string file_name);
    void accept(Visitor &visitor);
    void add(std::unique_ptr<ExternalDecl> edecl);

    void printAST();
    int semAnalyse();
    void opt();
    void codegen();

    void emitIR();
    void emitObj(std::string fileName);
};

#endif
