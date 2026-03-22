#include "Program.hpp"
#include "Visitor.hpp"
#include "CodegenVis.hpp"
#include <iostream>

void Program::accept(Visitor& visitor) {
    visitor.visitProgram(*this);
}

std::unique_ptr<Program> Program::optimize(OptimizeVisitor& optvis) {
    return std::move(optvis.visitProgram(*this));
}

void Program::add(std::unique_ptr<ExternalDecl> edecl) {
    root.push_back(std::move(edecl));
}

void Program::printAST() {
    PrintVisitor printvisitor;
    this->accept(printvisitor);
}

int Program::semAnalyse() {
    SemanticVisitor semvisitor;
    this->accept(semvisitor);
    return semvisitor.numOfErrors;
}

std::unique_ptr<Program> Program::optimize() {
    OptimizeVisitor optvis;
    return optvis.visitProgram(*this);
}

void Program::codegen() {
    codegenvis.initModule(fileName);

    for (int i = 0; i < root.size(); i++) {
        root[i]->codegen(codegenvis);
    }

    llvm::Module *mod = (codegenvis.Module).get();

    llvm::verifyModule(*mod, &llvm::errs());
}

void Program::printIR() {
    llvm::Module *mod = (codegenvis.Module).get();
    mod->print(llvm::outs(), nullptr);
}

void Program::emitObj(std::string fileName) {
    codegenvis.emitObj(fileName);
}

void Program::setFileName(std::string file_name) {
    fileName = file_name;
}
