#include "Error.hpp"
#include "Parser.hpp"
#include "Program.hpp"
#include "Scanner.hpp"
#include <iostream>

int main(int argc, char **argv) {
  bool optimize = 0;
  bool printAst = 0;
  bool printTokens = 0;
  bool emitIR = 0;
  bool notCompile = 0;
  std::string filename = "input.c";
  std::string destname = "output.o";

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
    } else if (flag == "-o") {
      destname = argv[++i];
      break;
    } else if (flag == "-z") {
      optimize = 1;
    } else if (flag == "-nc") {
      notCompile = 1;
    } else {
      std::cout << "Unknown Flag: " << argv[i] << "\n";
    }
  }

  getSourceLines(filename);

  Scanner scanner(filename);
  scanner.scanFile();
  scanner.scanProg();

  if (printTokens) {
    scanner.printTokens();
  }

  std::vector<Token> tokenlist = scanner.getTokenList();

  Parser parser(tokenlist);
  auto prog = parser.ParseProgram();
  prog->setFileName(filename);

  if (parser.numOfErrors > 0) {
    std::cout << "Build failed with " << parser.numOfErrors << " error(s)\n";
    return -1;
  }

  int noErr = prog->semAnalyse();

  if (printAst) {
    prog->printAST();
  }

  if (noErr > 0) {
    std::cout << "Build failed with " << noErr << " error(s)\n";
    return -1;
  }

  if (!notCompile) {
    prog->codegen();

    if (optimize) {
      prog->opt();
    }

    if (emitIR) {
      prog->printIR();
      prog->emitIR();
    }
    prog->emitObj(destname);
  }

  return 0;
}
