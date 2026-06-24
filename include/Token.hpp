#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <unordered_map>
#include "tabulate/table.hpp"

enum class TokenType {
    // keywords
    INT,
    CHAR,
    VOID,
    IF,
    ELSE,
    WHILE,
    RETURN,
    STRUCT,

    //access
    DOT,
    ARROW,

    // unary
    BANG,
    AMPERSAND,

    // operators
    PLUS,
    MINUS,
    ASTERISK,
    SLASH,
    MODULUS,
    EQUALS,
    LESS_THAN,
    GREATER_THAN,

    EQUALS_EQUALS,
    BANG_EQUALS,
    LESS_EQUALS,
    GREATER_EQUALS,

    AND,
    OR,

    // delimiters
    LEFT_CURLY,
    RIGHT_CURLY,
    LEFT_ROUND,
    RIGHT_ROUND,
    LEFT_SQUARE,
    RIGHT_SQUARE,
    COMMA,
    SEMICOLON,

    // literals
    IDENTIFIER,
    INTEGER,
    CHARACTER,
    STRING,

    END_OF_FILE
};

extern std::unordered_map<std::string, TokenType> keywords;

class Token {
  public:
    TokenType tokentype;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType tt, std::string lex, int line_num, int col_num);
    std::string getTokenStr();
    void printToken(tabulate::Table &table);
};

#endif
