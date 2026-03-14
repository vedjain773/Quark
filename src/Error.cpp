#include "Error.hpp"
#include <iostream>
#include <fstream>

std::vector<std::string> sourceLines;

void printErrorMsg(Error& error) {
    std::cout << "--> " << error.line << ":" << error.column << "\n";

    std::cout << error.line << "|" << sourceLines[error.line - 1] << "\n";

    for (int i = 0; i < error.column + 1; i++) {
        std::cout << " ";
    }
    std::cout << "^\n";

    std::cout << error.message << "\n";
}

void getSourceLines(std::string source) {
    std::ifstream file(source);

    std::string text;

    while (getline (file, text)) {
        sourceLines.push_back(text);
    }

    file.close();
}
