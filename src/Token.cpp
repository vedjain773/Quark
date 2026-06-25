#include "Token.hpp"
#include <iostream>

std::unordered_map<std::string, TokenType> keywords = {
    {"int", TokenType::INT},        {"uint8_t", TokenType::UINT8},
    {"uint16_t", TokenType::UINT16},{"char", TokenType::CHAR},
    {"void", TokenType::VOID},      {"if", TokenType::IF},
    {"else", TokenType::ELSE},      {"while", TokenType::WHILE},
    {"return", TokenType::RETURN},  {"struct", TokenType::STRUCT}};

std::string TokenTypeNames[]{
    // keywords
    "INT", "UINT8", "UINT16", "CHAR", "VOID", "IF", "ELSE", "WHILE", "RETURN", "STRUCT",

    // access
    "DOT", "ARROW",

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

void Token::printToken(tabulate::Table &table) {
    table.add_row(
        {lexeme, getTokenStr(), std::to_string(line), std::to_string(column)});
}
