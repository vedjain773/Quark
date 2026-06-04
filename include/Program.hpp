#ifndef PROGRAM_H
#define PROGRAM_H

#include "ExternalDecl.hpp"
#include "Statement.hpp"
#include "Visitor.hpp"
#include <memory>
#include <string>
#include <vector>

class Program {
public:
  std::vector<std::unique_ptr<ExternalDecl>> root;
  std::string fileName;
  CodegenVis codegenvis;

  void setFileName(std::string file_name);
  void accept(Visitor &visitor);
  void add(std::unique_ptr<ExternalDecl> edecl);

  void printAST();
  int semAnalyse();
  void opt();
  void codegen();

  void printIR();
  void emitIR();

  void emitObj(std::string fileName);
};

#endif
