#ifndef SCANNER_H
#define SCANNER_H

#include "frontend/scanner/Token.hpp"
#include <string>
#include <unordered_map>
#include <vector>

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

    void scanToken();

    char peekCurr();
    char peekNext();
    void getNextChar();
    void getNextLine();
    void addToken(const TokenType &tokenType);
    void checkChar();
    bool lookAhead(char expEnd);

    bool isDigit(char c);
    bool isAlpha(char c);

    void scanNumber();
    void scanWord();

  public:
    Scanner(std::string sourceArg);
    void scanFile();
    void scanProg();

    std::vector<Token> getTokenList();
    void printTokens();
};

#endif
