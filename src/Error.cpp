#include "Error.hpp"
#include <iostream>
#include <fstream>

#define RED   "\033[31m"
#define YELLOW "\033[33m"
#define BLUE  "\033[34m"
#define BOLD  "\033[1m"
#define RESET "\033[0m"

std::vector<std::string> sourceLines;

void printErrorMsg(Error& error) {
    std::cout << "--> " << error.line << ":" << error.column << "\n";

    std::cout << error.line << "|" << sourceLines[error.line - 1] << "\n";

    for (int i = 0; i < error.column + 1; i++) {
        std::cout << " ";
    }
    std::cout << "^\n";

    std::cout << RED << "[ERROR]" << RESET << " ";
    std::cout << error.message << "\n\n";
}

void getSourceLines(std::string source) {
    std::ifstream file(source);

    std::string text;

    while (getline (file, text)) {
        sourceLines.push_back(text);
    }

    file.close();
}
