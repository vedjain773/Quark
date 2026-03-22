#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Function.hpp"
#include "Token.hpp"
#include "Expression.hpp"
#include "Statement.hpp"
#include "Program.hpp"

class Parser {
    public:
    std::vector<Token> TokenList;
    int current;
    int numOfErrors = 0;

    Parser(std::vector<Token> tokenlist);
    Token getNextToken();
    Token peekCurr();
    Token peekNext();

    //Parse Expressions
    std::unique_ptr<Expression> ParseIntExpr();
    std::unique_ptr<Expression> ParseCharExpr();
    std::unique_ptr<Expression> ParseVarExpr();
    std::unique_ptr<Expression> ParseParenExpr();
    std::unique_ptr<Expression> ParsePrimaryExpr();
    std::unique_ptr<Expression> ParseCallExpr();
    std::unique_ptr<Expression> ParseUnaryExpr();
    std::unique_ptr<Expression> ParseFactorExpr();
    std::unique_ptr<Expression> ParseTermExpr();
    std::unique_ptr<Expression> ParseCompExpr();
    std::unique_ptr<Expression> ParseEqualityExpr();
    std::unique_ptr<Expression> ParseLAndExpr();
    std::unique_ptr<Expression> ParseLOrExpr();
    std::unique_ptr<Expression> ParseAssignExpr();
    std::unique_ptr<Expression> ParseExpr();

    //Parse Statements
    std::unique_ptr<Statement> ParseExprStmt();
    std::unique_ptr<BlockStmt> ParseBlockStmt();
    std::unique_ptr<Statement> ParseIfStmt();
    std::unique_ptr<Statement> ParseElseStmt();
    std::unique_ptr<Statement> ParseWhileStmt();
    std::unique_ptr<Statement> ParseReturnStmt();
    std::unique_ptr<Statement> ParseDeclStmt();
    std::unique_ptr<Statement> ParseStmt();

    //Parse Functions
    std::unique_ptr<Parameter> ParseParameter();
    std::unique_ptr<Prototype> ParsePrototype();
    std::unique_ptr<FuncDef> ParseFuncDef();

    std::unique_ptr<Program> ParseProgram();
};

#endif
