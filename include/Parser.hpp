#ifndef PARSER_H
#define PARSER_H

#include "Expression.hpp"
#include "Function.hpp"
#include "Program.hpp"
#include "Statement.hpp"
#include "Token.hpp"
#include <tuple>
#include <vector>

enum BinOpPrec { FACTOR, TERM, COMP, COMP_EQL, LAND, LOR, MISC = 100 };

BinOpPrec getBinPrecedence(Operators Op);
bool isPostFixOp(TokenType tokenType);
bool isAssignOp(TokenType tokenType);

class Parser {
  private:
    std::vector<Token> TokenList;
    int current;

    Token getNextToken();
    Token peekCurr();
    Token peekNext();
    Token peekPrev();
    Token peekAhead(int n);

    void advToSyncPoint();

    std::tuple<TypeKind *, std::string> ParseTypePrefix();
    TypeKind *ParseTypeSuffix(TypeKind *typek, std::string typeName);
    TypeKind *ParseType();
    std::tuple<TypeKind *, std::string, int, int> getTypeNamePair();

    void expect(const Token &token, std::string msg);
    void expect(int line, int col, std::string msg);

    // Parse Expressions
    std::unique_ptr<Expression> ParseIntExpr();
    std::unique_ptr<Expression> ParseCharExpr();
    std::unique_ptr<Expression> ParseVarExpr();
    std::unique_ptr<Expression> ParseParenExpr();
    std::unique_ptr<Expression> ParsePrimaryExpr();
    std::unique_ptr<Expression> ParsePostFixExpr();
    std::unique_ptr<Expression> ParseDerefExpr();
    std::unique_ptr<Expression> ParseAddressExpr();
    std::unique_ptr<Expression> ParseSizeOfExpr();
    std::unique_ptr<Expression> ParseUnaryExpr();
    std::unique_ptr<Expression> ParseBinExpr(BinOpPrec level);
    std::unique_ptr<Expression> ParseAssignExpr();
    std::unique_ptr<Expression> ParseExpr();

    // Parse Statements
    std::unique_ptr<Statement> ParseExprStmt();
    std::unique_ptr<BlockStmt> ParseBlockStmt();
    std::unique_ptr<Statement> ParseIfStmt();
    std::unique_ptr<Statement> ParseElseStmt();
    std::unique_ptr<Statement> ParseWhileStmt();
    std::unique_ptr<Statement> ParseForStmt();
    std::unique_ptr<Statement> ParseReturnStmt();
    std::unique_ptr<Statement> ParseDeclStmt();
    std::unique_ptr<Statement> ParseStmt();

    // Parse Structs
    std::unique_ptr<StructField> ParseStructField();
    std::unique_ptr<StructDecl> ParseStructDecl();

    // Parse Functions
    std::unique_ptr<Parameter> ParseParameter();
    std::unique_ptr<Prototype> ParsePrototype();
    std::unique_ptr<FuncDef> ParseFuncDef();

  public:
    int numOfErrors = 0;

    Parser(std::vector<Token> tokenlist);
    std::unique_ptr<Program> ParseProgram();
};

#endif
