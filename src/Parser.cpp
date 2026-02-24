#include "Parser.hpp"
#include "Visitor.hpp"
#include "Error.hpp"
#include "Program.hpp"
#include <iostream>

Parser::Parser(std::vector<Token> tokenlist) {
    TokenList = tokenlist;
    current = 0;
}

Token Parser::getNextToken() {
    if (current < TokenList.size()) {
        return TokenList[current++];
    } else {
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
}

Token Parser::peekCurr() {
    return TokenList[current];
}

Token Parser::peekNext() {
    return TokenList[current+1];
}

std::unique_ptr<Expression> Parser::ParseIntExpr() {
    if (peekCurr().tokentype != TokenType::INTEGER) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected INTEGER, got " + peekCurr().getTokenStr());
        return nullptr;
    }

    std::string NumStr = peekCurr().lexeme;
    int NumVal = std::stoi(NumStr);

    auto Result = std::make_unique<IntExpr>(NumVal, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseCharExpr() {
    if (peekCurr().tokentype != TokenType::CHARACTER) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected CHARACTER, got " + peekCurr().getTokenStr());
        return nullptr;
    }

    std::string charStr = peekCurr().lexeme;
    char charac = charStr[1];

    auto Result = std::make_unique<CharExpr>(charac, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseVarExpr() {
    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected IDENTIFER, got " + peekCurr().getTokenStr());
        return nullptr;
    }

    std::string Var = peekCurr().lexeme;

    auto Result = std::make_unique<VarExpr>(Var, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseParenExpr() {
    getNextToken();

    auto Result = ParseLOrExpr();

    if (!Result) {
        return nullptr;
    }

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Missing ')'");
    } else {
        getNextToken();
    }

    return Result;
}

std::unique_ptr<Expression> Parser::ParsePrimaryExpr() {
    switch(peekCurr().tokentype) {
        case TokenType::INTEGER: {
            return ParseIntExpr();
        }
        break;

        case TokenType::CHARACTER: {
            return ParseCharExpr();
        }

        case TokenType::IDENTIFIER: {
            return ParseVarExpr();
        }
        break;

        case TokenType::LEFT_ROUND: {
            return ParseParenExpr();
        }
        break;

        default: {
            Error error(peekCurr().line, peekCurr().column);
            error.printErrorMsg("Invalid token: '" + peekCurr().lexeme + "'");
            return nullptr;
        }
    }
}

std::unique_ptr<Expression> Parser::ParseCallExpr() {
    std::string name = peekCurr().lexeme;
    int line = peekCurr().line;
    int column = peekCurr().column;
    auto Prim = ParsePrimaryExpr();

    if (peekCurr().tokentype != TokenType:: LEFT_ROUND) {
        return Prim;
    } else {
        getNextToken();

        auto Result = std::make_unique<CallExpr>(name, line, column);
        while (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
            auto var = ParseExpr();
            Result->add(std::move(var));

            if (peekCurr().tokentype == TokenType::COMMA) {
                getNextToken();
            }
        }
        getNextToken();
        return Result;
    }
}

std::unique_ptr<Expression> Parser::ParseUnaryExpr() {
    if (peekCurr().tokentype == TokenType::BANG || peekCurr().tokentype == TokenType::MINUS) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();

        auto Result = std::make_unique<UnaryExpr>(oper, ParseUnaryExpr(), tline, tcol);
        return Result;
    } else {
        return ParseCallExpr();
    }
}

std::unique_ptr<Expression> Parser::ParseFactorExpr() {
    auto lhs = std::move(ParseUnaryExpr());

    while (peekCurr().tokentype == TokenType::SLASH || peekCurr().tokentype == TokenType::ASTERISK || peekCurr().tokentype == TokenType::MODULUS) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseUnaryExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseTermExpr() {
    auto lhs = std::move(ParseFactorExpr());

    while (peekCurr().tokentype == TokenType::MINUS || peekCurr().tokentype == TokenType::PLUS) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseFactorExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseCompExpr() {
    auto lhs = std::move(ParseTermExpr());

    while (peekCurr().tokentype == TokenType::GREATER_THAN ||
            peekCurr().tokentype == TokenType::GREATER_EQUALS ||
            peekCurr().tokentype == TokenType::LESS_EQUALS ||
            peekCurr().tokentype == TokenType::LESS_THAN) {

        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseTermExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseEqualityExpr() {
    auto lhs = std::move(ParseCompExpr());

    while (peekCurr().tokentype == TokenType::BANG_EQUALS || peekCurr().tokentype == TokenType::EQUALS_EQUALS) {

        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseCompExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseLAndExpr() {
    auto lhs = std::move(ParseEqualityExpr());

    while (peekCurr().tokentype == TokenType::AND) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseEqualityExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseLOrExpr() {
    auto lhs = std::move(ParseLAndExpr());

    while (peekCurr().tokentype == TokenType::OR) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseLAndExpr());
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs), tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseAssignExpr() {
    auto lhs = std::move(ParseLOrExpr());

    if (peekCurr().tokentype == TokenType::EQUALS) {
        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = std::move(ParseAssignExpr());

        auto Result = std::make_unique<AssignExpr>(std::move(lhs), std::move(rhs), tline, tcol);
        return Result;
    } else {
        return lhs;
    }
}

std::unique_ptr<Expression> Parser::ParseExpr() {
    return ParseAssignExpr();
}

std::unique_ptr<Statement> Parser::ParseExprStmt() {
    std::unique_ptr<Expression> expr = ParseExpr();

    if (peekCurr().tokentype != TokenType::SEMICOLON) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Missing ';'");
        return nullptr;
    } else {
        getNextToken();
        auto Result = std::make_unique<ExprStmt>(std::move(expr));
        return Result;
    }
}

std::unique_ptr<BlockStmt> Parser::ParseBlockStmt() {
    getNextToken();
    auto Result = std::make_unique<BlockStmt>();

    while (peekCurr().tokentype != TokenType::RIGHT_CURLY) {
        auto stmt = ParseStmt();
        Result->addStmt(std::move(stmt));

        if (peekCurr().tokentype == TokenType::END_OF_FILE) {
            Error error(peekCurr().line, peekCurr().column);
            error.printErrorMsg("Expected '}'" + peekCurr().lexeme);
            return nullptr;
        }
    }

    getNextToken();
    return Result;
}

std::unique_ptr<Statement> Parser::ParseIfStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected '('");
        return nullptr;
    }

    getNextToken();
    auto condn = ParseLOrExpr();

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Missing ')'");
        return nullptr;
    }
    getNextToken();

    auto ifbody = ParseBlockStmt();

    if (peekCurr().tokentype == TokenType::ELSE) {
        auto elsestmt = ParseElseStmt();
        auto Result = std::make_unique<IfStmt>(std::move(condn), std::move(ifbody), std::move(elsestmt));
        return Result;
    } else {
        auto elsestmt = nullptr;
        auto Result = std::make_unique<IfStmt>(std::move(condn), std::move(ifbody), std::move(elsestmt));
        return Result;
    }
}

std::unique_ptr<Statement> Parser::ParseElseStmt() {
    getNextToken();

    auto elsebody = ParseBlockStmt();
    auto Result = std::make_unique<ElseStmt>(std::move(elsebody));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseWhileStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected '('");
        return nullptr;
    }

    getNextToken();
    auto condn = ParseLOrExpr();

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Missing ')'");
        return nullptr;
    }
    getNextToken();

    auto whilebody = ParseBlockStmt();
    auto Result = std::make_unique<WhileStmt>(std::move(condn), std::move(whilebody));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseReturnStmt() {
    getNextToken();

    if (peekCurr().tokentype == TokenType::SEMICOLON) {
        auto retexpr = std::make_unique<EmptyExpr>();
        retexpr->line = peekCurr().line;
        retexpr->column = peekCurr().column;
        auto Result = std::make_unique<ReturnStmt>(std::move(retexpr));

        getNextToken();
        return Result;
    } else {
        auto retexpr = ParseExpr();
        auto Result = std::make_unique<ReturnStmt>(std::move(retexpr));

        getNextToken();
        return Result;
    }
}

std::unique_ptr<Statement> Parser::ParseDeclStmt() {
    TokenType tokenType = peekCurr().tokentype;

    getNextToken();

    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected IDENTIFIER");
        return nullptr;
    }

    std::string varname = peekCurr().lexeme;

    int tline = peekCurr().line;
    int tcol = peekCurr().column;

    getNextToken();

    if (peekCurr().tokentype == TokenType::EQUALS) {
        getNextToken();
        auto expr = ParseLOrExpr();
        auto Result = std::make_unique<DeclStmt>(tokenType, varname, std::move(expr), tline, tcol);
        getNextToken();
        return Result;
    } else {
        auto Result = std::make_unique<DeclStmt>(tokenType, varname, nullptr, tline, tcol);
        getNextToken();
        return Result;
    }
}

std::unique_ptr<Parameter> Parser::ParseParameter() {
    TokenType type = peekCurr().tokentype;

    if (type != TokenType::INT && type != TokenType::CHAR) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected Datatype got: " + peekCurr().lexeme);
        return nullptr;
    }

    getNextToken();

    std::string name = peekCurr().lexeme;
    getNextToken();

    auto Result = std::make_unique<Parameter>(type, name);
    return Result;
}

std::unique_ptr<Prototype> Parser::ParsePrototype() {
    TokenType type = peekCurr().tokentype;
    int line = peekCurr().line;
    int column = peekCurr().column;
    getNextToken();

    std::string name = peekCurr().lexeme;
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column);
        error.printErrorMsg("Expected (");
        return nullptr;
    }
    getNextToken();

    auto Result = std::make_unique<Prototype>(type, name, line, column);
    while(peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        auto param = ParseParameter();

        if (param != nullptr) {
            Result->addParam(std::move(param));

            if (peekCurr().tokentype == TokenType::COMMA) {
                getNextToken();
            }
        }
    }

    getNextToken();
    return Result;
}

std::unique_ptr<FuncDef> Parser::ParseFuncDef() {
    auto proto = ParsePrototype();
    auto body = ParseBlockStmt();

    auto Result = std::make_unique<FuncDef>(std::move(proto), std::move(body));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseStmt() {
    switch(peekCurr().tokentype) {
        case TokenType::LEFT_CURLY: {
            return ParseBlockStmt();
        }
        break;

        case TokenType::INT:
        case TokenType::CHAR: {
            return ParseDeclStmt();
        }
        break;

        case TokenType::IF: {
            return ParseIfStmt();
        }
        break;

        case TokenType::WHILE: {
            return ParseWhileStmt();
        }
        break;

        case TokenType::RETURN: {
            return ParseReturnStmt();
        }
        break;

        default: {
            return ParseExprStmt();
        }
    }
}

Program Parser::ParseProgram() {
    Program program;
    while (peekCurr().tokentype != TokenType::END_OF_FILE) {
        auto edecl = ParseFuncDef();
        program.add(std::move(edecl));
    }

    return program;
}
