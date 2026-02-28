#include "Program.hpp"
#include "Visitor.hpp"
#include "CodegenVis.hpp"
#include <iostream>

void Program::accept(Visitor& visitor) {
    visitor.visitProgram(*this);
}

void Program::add(std::unique_ptr<ExternalDecl> edecl) {
    root.push_back(std::move(edecl));
}

void Program::printAST() {
    PrintVisitor printvisitor;
    this->accept(printvisitor);
}

void Program::semAnalyse() {
    SemanticVisitor semvisitor;
    this->accept(semvisitor);
}

void Program::codegen() {
    std::cout << "\n";
    codegenvis.initModule();

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
