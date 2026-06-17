#include "Token.hpp"
#include <iostream>

std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::INT},       {"char", TokenType::CHAR},
    {"void", TokenType::VOID},     {"if", TokenType::IF},
    {"else", TokenType::ELSE},     {"while", TokenType::WHILE},
    {"return", TokenType::RETURN}, {"struct", TokenType::STRUCT}};

std::string TokenTypeNames[]{
    // keywords
    "INT", "CHAR", "VOID", "IF", "ELSE", "WHILE", "RETURN", "STRUCT",

    // unary
    "BANG", "AMPERSAND",

    // operators
    "PLUS", "MINUS", "ASTERISK", "SLASH", "MODULUS", "EQUALS", "LESS_THAN",
    "GREATER_THAN",

    "EQUALS_EQUALS", "BANG_EQUALS", "LESS_EQUALS", "GREATER_EQUALS",

    "AND", "OR",

    // delimiters
    "LEFT_CURLY", "RIGHT_CURLY", "LEFT_ROUND", "RIGHT_ROUND", "LEFT_SQUARE",
    "RIGHT_SQUARE", "COMMA", "SEMICOLON",

    // literals
    "IDENTIFIER", "INTEGER", "CHARACTER", "STRING",

    "END_OF_FILE"};

Token::Token(TokenType token_type, std::string lex, int line_num, int col_num) {
    tokentype = token_type;
    lexeme = lex;
    line = line_num;
    column = col_num;
}

std::string Token::getTokenStr() {
    return TokenTypeNames[static_cast<size_t>(tokentype)];
}

void Token::printToken() {
    std::cout << "Token: " << lexeme << " Type: " << getTokenStr()
              << " line: " << line << " column: " << column;
}
