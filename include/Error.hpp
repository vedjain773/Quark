#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>

struct Error {
    int line;
    int column;
    std::string message;

    Error(int l, int c, std::string msg): line(l), column(c), message(msg) {}
};

extern std::vector<std::string> sourceLines;

void printErrorMsg(Error& error);
void getSourceLines(std::string source);

#endif
