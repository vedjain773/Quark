#include "Error.hpp"
#include <fstream>
#include <iostream>

#define RED "\033[31m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

std::vector<std::string> sourceLines;

Error::Error(int l, int c, std::string msg) : line(l), column(c), message(msg) {
  printErrorMsg(*this);
}

void printErrorMsg(Error &error) {
  std::cout << "--> " << error.line << ":" << error.column << "\n";
  
  std::string msg;

  if (error.line - 1 == sourceLines.size())
    msg = "END OF FILE";
  else
    msg = sourceLines[error.line - 1];

  std::cout << error.line << "|" << msg << "\n";

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

  while (getline(file, text)) {
    sourceLines.push_back(text);
  }

  file.close();
}
