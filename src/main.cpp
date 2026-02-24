#include "Scanner.hpp"
#include "Parser.hpp"
#include "Program.hpp"

int main() {
    Scanner scanner("test/sample.c");
    scanner.scanFile();
    scanner.scanProg();
    // scanner.printTokens();

    std::vector <Token> tokenlist = scanner.getTokenList();
    Parser parser(tokenlist);

    Program prog = parser.ParseProgram();
    prog.semAnalyse();
    prog.printAST();
    prog.codegen();

    return 0;
}
