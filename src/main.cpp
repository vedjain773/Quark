#include "Scanner.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include <iostream>

int main(int argc, char** argv) {
    bool printAst = 0;
    bool printTokens = 0;
    bool emitIR = 0;
    std::string filename = "sample.c";
    std::string destname = "sample.o";

    if (argc == 1) {
        std::cout << "Usage: \n";
        std::cout << "./minic <src> <flag> <dest?>\n";
        return 0;
    }

    filename = argv[1];

    for (int i = 2; i < argc; i++) {
        std::string flag = argv[i];

        if (flag == "--pt") {
            printTokens = true;
        } else if (flag == "--past") {
            printAst = true;
        } else if (flag == "--emit-ir") {
            emitIR = true;
        }else if (flag == "-o") {
            destname = argv[++i];
            break;
        } else {
            std::cout << "Unknown Flag: " << argv[i] << "\n";
        }
    }

    Scanner scanner(filename);
    scanner.scanFile();
    scanner.scanProg();

    if (printTokens) {
        scanner.printTokens();
    }

    std::vector <Token> tokenlist = scanner.getTokenList();
    Parser parser(tokenlist);
    Program prog = parser.ParseProgram();
    prog.semAnalyse();

    if (printAst) {
        prog.printAST();
    }

    prog.codegen();

    if (emitIR) {
        prog.printIR();
    }

    prog.emitObj(destname);

    return 0;
}
