#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <vector>

struct Error {
    int line;
    int column;
    std::string message;

    Error(int l, int c, const std::string &msg);
};

struct Warning {
    int line;
    int column;
    std::string message;

    Warning(int l, int c, const std::string &msg);
};

extern std::vector<std::string> sourceLines;

void printErrorMsg(Error &error);
void printWarning(Warning &warning);

void getSourceLines(const std::string &source);
int getNumDig(int line);

#endif
