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
    CodegenVis codegenvis;
    codegenvis.initModule();

    for (int i = 0; i < root.size(); i++) {
        root[i]->codegen(codegenvis);
    }

    llvm::Module *mod = (codegenvis.Module).get();

    llvm::verifyModule(*mod, &llvm::errs());
    mod->print(llvm::outs(), nullptr);

    codegenvis.emitObj("out/sample.o");
}
