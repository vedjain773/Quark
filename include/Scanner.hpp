#ifndef SCANNER_H
#define SCANNER_H

#include "Token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Scanner {
    private:
    std::string source;
    std::string sourceContent;
    std::vector<Token> tokenList;

    int start = 0;
    int current = 0;
    int line = 1;
    int column = 1;
    int tokStartCol = 1;

    public:
    Scanner (std::string sourceArg);
    void scanFile();
    void scanProg();
    void scanToken();
    void printTokens();
    std::vector<Token> getTokenList();

    char peekCurr();
    char peekNext();
    void getNextChar();
    void getNextLine();
    void addToken(TokenType tokenType);
    void checkChar();
    bool lookAhead(char expEnd);

    bool isDigit(char c);
    bool isAlpha(char c);

    void scanNumber();
    void scanWord();
};

#endif
