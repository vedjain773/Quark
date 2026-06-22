#include "Parser.hpp"
#include "Error.hpp"
#include "Program.hpp"
#include "Visitor.hpp"
#include <iostream>

int getBinPrecedence(Operators Op) {
    switch (Op) {
    case Operators::MULT:
    case Operators::DIVIDE:
    case Operators::MODULUS:
        return 0;
        break;

    case Operators::PLUS:
    case Operators::MINUS:
        return 10;
        break;

    case Operators::GREATER:
    case Operators::GREATER_EQUALS:
    case Operators::LESS:
    case Operators::LESS_EQUALS:
        return 20;
        break;

    case Operators::EQUALS:
    case Operators::NOT_EQUALS:
        return 30;
        break;

    case Operators::AND:
        return 40;
        break;

    case Operators::OR:
        return 50;
        break;

    default:
        return 100;
    }
}

bool isPostFixOp(TokenType tokenType) {
    switch (tokenType) {
    case TokenType::LEFT_ROUND:
    case TokenType::LEFT_SQUARE:
        return true;
    break;

    default: return false;
    }
}

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

Token Parser::peekCurr() { return TokenList[current]; }

Token Parser::peekNext() { return TokenList[current + 1]; }

std::unique_ptr<Expression> Parser::ParseIntExpr() {
    std::string NumStr = peekCurr().lexeme;
    int NumVal = std::stoi(NumStr);

    auto Result =
        std::make_unique<IntExpr>(NumVal, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseCharExpr() {
    std::string charStr = peekCurr().lexeme;
    char charac = charStr[1];

    auto Result =
        std::make_unique<CharExpr>(charac, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseVarExpr() {
    std::string Var = peekCurr().lexeme;

    auto Result =
        std::make_unique<VarExpr>(Var, peekCurr().line, peekCurr().column);
    getNextToken();
    return std::move(Result);
}

std::unique_ptr<Expression> Parser::ParseParenExpr() {
    getNextToken();

    auto Result = ParseBinExpr(50);

    if (!Result) {
        return nullptr;
    }

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Missing ')'");
        numOfErrors += 1;
    } else {
        getNextToken();
    }

    return Result;
}

std::unique_ptr<Expression> Parser::ParsePrimaryExpr() {
    switch (peekCurr().tokentype) {
    case TokenType::INTEGER: {
        return ParseIntExpr();
    } break;

    case TokenType::CHARACTER: {
        return ParseCharExpr();
    }

    case TokenType::IDENTIFIER: {
        return ParseVarExpr();
    } break;

    case TokenType::LEFT_ROUND: {
        return ParseParenExpr();
    } break;

    default: {
        Error error(peekCurr().line, peekCurr().column, "Expected Expression");
        numOfErrors += 1;
        return nullptr;
    }

    }
}

std::unique_ptr<Expression> Parser::ParsePostFixExpr() {
    std::string name = peekCurr().lexeme;
    int line = peekCurr().line;
    int column = peekCurr().column;
    auto Prim = ParsePrimaryExpr();

    while (isPostFixOp(peekCurr().tokentype)) {
        switch (peekCurr().tokentype) {
        case TokenType::LEFT_ROUND: {
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
            Prim = std::move(Result);
        } break;

        case TokenType::LEFT_SQUARE: {
            getNextToken();

            auto inner = ParseExpr();

            auto binExpr = std::make_unique<BinaryExpr>(
                    Operators::PLUS, std::move(Prim),
                    std::move(inner), line, column);

            auto Result = std::make_unique<DerefExpr>(std::move(binExpr), line, column);

            getNextToken();

            Prim = std::move(Result); 
        } break;    
        
        default: return nullptr;
        }
    }

    return Prim;
}

std::unique_ptr<Expression> Parser::ParseDerefExpr() {
    int line = peekCurr().line;
    int column = peekCurr().column;

    getNextToken();
    auto expr = ParseUnaryExpr();

    auto result = std::make_unique<DerefExpr>(std::move(expr), line, column);
    return result;
}

std::unique_ptr<Expression> Parser::ParseAddressExpr() {
    int line = peekCurr().line;
    int column = peekCurr().column;

    getNextToken();
    auto expr = ParseUnaryExpr();

    auto result = std::make_unique<AddressExpr>(std::move(expr), line, column);

    return result;
}

std::unique_ptr<Expression> Parser::ParseUnaryExpr() {
    switch (peekCurr().tokentype) {
    case TokenType::BANG:
    case TokenType::MINUS: {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();

        auto Result =
            std::make_unique<UnaryExpr>(oper, ParseUnaryExpr(), tline, tcol);
        return Result;
    } break;

    case TokenType::ASTERISK: {
        return ParseDerefExpr();
    } break;

    case TokenType::AMPERSAND: {
        return ParseAddressExpr();
    } break;

    default:
        return ParsePostFixExpr();
    }
}

std::unique_ptr<Expression> Parser::ParseBinExpr(int level) {
    auto parseOperand = [&](int level) {
        if (level == 0)
            return ParseUnaryExpr();
        else
            return ParseBinExpr(level - 10);
    };

    auto lhs = parseOperand(level);

    while (getBinPrecedence(getOp(peekCurr().lexeme)) == level) {
        Operators oper = getOp(peekCurr().lexeme);

        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = parseOperand(level);
        lhs = std::make_unique<BinaryExpr>(oper, std::move(lhs), std::move(rhs),
                                           tline, tcol);
    }

    return lhs;
}

std::unique_ptr<Expression> Parser::ParseAssignExpr() {
    auto lhs = ParseBinExpr(50);

    if (peekCurr().tokentype == TokenType::EQUALS) {
        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = ParseAssignExpr();

        auto Result = std::make_unique<AssignExpr>(std::move(lhs),
                                                   std::move(rhs), tline, tcol);
        return Result;
    } else {
        return lhs;
    }
}

std::unique_ptr<Expression> Parser::ParseExpr() { return ParseAssignExpr(); }

std::unique_ptr<Statement> Parser::ParseExprStmt() {
    std::unique_ptr<Expression> expr = ParseExpr();

    if (peekCurr().tokentype != TokenType::SEMICOLON) {
        Error error(peekCurr().line, peekCurr().column, "Missing ';'");
        numOfErrors += 1;
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

        if (stmt)
            Result->addStmt(std::move(stmt));

        if (peekCurr().tokentype == TokenType::END_OF_FILE) {
            Error error(peekCurr().line, peekCurr().column,
                        "Expected '}'" + peekCurr().lexeme);
            numOfErrors += 1;
            return nullptr;
        }
    }

    getNextToken();
    return Result;
}

std::unique_ptr<Statement> Parser::ParseIfStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Expected '('");
        numOfErrors += 1;
        return nullptr;
    }

    getNextToken();
    auto condn = ParseBinExpr(50);

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Missing ')'");
        numOfErrors += 1;
        return nullptr;
    }
    getNextToken();

    auto ifbody = ParseStmt();

    if (peekCurr().tokentype == TokenType::ELSE) {
        auto elsestmt = ParseElseStmt();
        auto Result = std::make_unique<IfStmt>(
            std::move(condn), std::move(ifbody), std::move(elsestmt));
        return Result;
    } else {
        auto elsestmt = nullptr;
        auto Result = std::make_unique<IfStmt>(
            std::move(condn), std::move(ifbody), std::move(elsestmt));
        return Result;
    }
}

std::unique_ptr<Statement> Parser::ParseElseStmt() {
    getNextToken();

    auto elsebody = ParseStmt();
    auto Result = std::make_unique<ElseStmt>(std::move(elsebody));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseWhileStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Expected '('");
        numOfErrors += 1;
        return nullptr;
    }

    getNextToken();
    auto condn = ParseBinExpr(50);

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Missing ')'");
        numOfErrors += 1;
        return nullptr;
    }
    getNextToken();

    auto whilebody = ParseStmt();
    auto Result =
        std::make_unique<WhileStmt>(std::move(condn), std::move(whilebody));
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
    std::string typeName = peekCurr().lexeme;

    getNextToken();

    TypeKind *typek = getType(typeName);
    while (peekCurr().tokentype == TokenType::ASTERISK) {
        typeName += '*';
        typek = getType(typeName);

        getNextToken();
    }

    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        Error error(peekCurr().line, peekCurr().column, "Expected IDENTIFIER");
        numOfErrors += 1;
        return nullptr;
    }

    std::string varname = peekCurr().lexeme;

    int tline = peekCurr().line;
    int tcol = peekCurr().column;

    getNextToken();

    while (peekCurr().tokentype == TokenType::LEFT_SQUARE) {
        getNextToken();

        auto iExpr = ParseIntExpr();
        IntExpr *intExpr = static_cast<IntExpr *>(iExpr.get());
        
        if (peekCurr().tokentype != TokenType::RIGHT_SQUARE) {
            Error error(peekCurr().line, peekCurr().column, "Expected ']'");
            numOfErrors += 1;
            return nullptr;
        }

        getNextToken();
        typek = getArrType(typeName, intExpr->Val);
        typeName = typek->name;
    }

    if (peekCurr().tokentype == TokenType::EQUALS) {
        getNextToken();
        auto expr = ParseBinExpr(50);
        auto Result = std::make_unique<DeclStmt>(typek, varname,
                                                 std::move(expr), tline, tcol);
        getNextToken();
        return Result;
    } else {
        auto Result =
            std::make_unique<DeclStmt>(typek, varname, nullptr, tline, tcol);
        getNextToken();
        return Result;
    }
}

std::unique_ptr<StructField> Parser::ParseStructField() {
    std::string typeName = peekCurr().lexeme;

    getNextToken();

    TypeKind *typek = getType(typeName);
    while (peekCurr().tokentype == TokenType::ASTERISK) {
        typeName += '*';
        typek = getType(typeName);

        getNextToken();
    }

    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        Error error(peekCurr().line, peekCurr().column, "Expected IDENTIFIER");
        numOfErrors += 1;
        return nullptr;
    }

    std::string fieldName = peekCurr().lexeme;

    int tline = peekCurr().line;
    int tcol = peekCurr().column;

    getNextToken();

    while (peekCurr().tokentype == TokenType::LEFT_SQUARE) {
        getNextToken();

        auto iExpr = ParseIntExpr();
        IntExpr *intExpr = static_cast<IntExpr *>(iExpr.get());
        
        if (peekCurr().tokentype != TokenType::RIGHT_SQUARE) {
            Error error(peekCurr().line, peekCurr().column, "Expected ']'");
            numOfErrors += 1;
            return nullptr;
        }

        getNextToken();
        typek = getArrType(typeName, intExpr->Val);
        typeName = typek->name;
    }
    
    return std::make_unique<StructField>(typek, fieldName, tline, tcol);
}

std::unique_ptr<Statement> Parser::ParseStructDecl() {
    int line = peekCurr().line;
    int column = peekCurr().column;
    getNextToken();

    std::string tag = peekCurr().lexeme;
    auto Result = std::make_unique<StructDecl>(tag, line, column);
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_CURLY) {
        Error error(peekCurr().line, peekCurr().column, "Expected: '{'");
        numOfErrors += 1;
        return nullptr;
    }

    getNextToken();

    while (peekCurr().tokentype != TokenType::RIGHT_CURLY) {
        auto structField = ParseStructField();
        Result->addField(std::move(structField));
        getNextToken();
    }

    //Consume }
    getNextToken();

    //Consume ;
    getNextToken();

    return Result;
}

std::unique_ptr<Parameter> Parser::ParseParameter() {
    TokenType type = peekCurr().tokentype;
    std::string typeName = peekCurr().lexeme;

    if (type != TokenType::INT && type != TokenType::CHAR) {
        Error error(peekCurr().line, peekCurr().column,
                    "Expected Datatype got: " + peekCurr().lexeme);
        numOfErrors += 1;
        return nullptr;
    }

    getNextToken();

    TypeKind *typek = getType(typeName);
    while (peekCurr().tokentype == TokenType::ASTERISK) {
        typeName += '*';
        typek = getType(typeName);

        getNextToken();
    }

    std::string name = peekCurr().lexeme;
    getNextToken();

    auto Result = std::make_unique<Parameter>(typek, name);
    return Result;
}


std::unique_ptr<Prototype> Parser::ParsePrototype() {
    std::string typeName = peekCurr().lexeme;

    int line = peekCurr().line;
    int column = peekCurr().column;
    getNextToken();

    TypeKind *typek = getType(typeName);
    while (peekCurr().tokentype == TokenType::ASTERISK) {
        typeName += '*';
        typek = getType(typeName);

        getNextToken();
    }

    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        Error error(peekCurr().line, peekCurr().column,
                    "Expected an IDENTIFIER");
        numOfErrors += 1;
        return nullptr;
    }

    std::string name = peekCurr().lexeme;
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        Error error(peekCurr().line, peekCurr().column, "Expected (");
        numOfErrors += 1;
        return nullptr;
    }
    getNextToken();

    auto Result = std::make_unique<Prototype>(typek, name, line, column);
    while (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        auto param = ParseParameter();

        if (param != nullptr) {
            Result->addParam(std::move(param));

            if (peekCurr().tokentype == TokenType::COMMA) {
                getNextToken();
            } else if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
                Error error(peekCurr().line, peekCurr().column, "Expected ,");
                numOfErrors += 1;
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
    switch (peekCurr().tokentype) {
    case TokenType::RIGHT_CURLY: {
        Error error(peekCurr().line, peekCurr().column, "Unexpected '}'");
        return nullptr;
    } break;

    case TokenType::RIGHT_ROUND: {
        Error error(peekCurr().line, peekCurr().column, "Unexpected ')'");
        return nullptr;
    } break;

    case TokenType::LEFT_CURLY: {
        return ParseBlockStmt();
    } break;

    case TokenType::INT:
    case TokenType::CHAR: {
        return ParseDeclStmt();
    } break;

    case TokenType::STRUCT: {
        return ParseStructDecl();
    } break;

    case TokenType::IF: {
        return ParseIfStmt();
    } break;

    case TokenType::WHILE: {
        return ParseWhileStmt();
    } break;

    case TokenType::RETURN: {
        return ParseReturnStmt();
    } break;

    default: {
        return ParseExprStmt();
    }
    }
}

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while (peekCurr().tokentype != TokenType::END_OF_FILE) {
        auto edecl = ParseFuncDef();
        program->add(std::move(edecl));
    }

    return program;
}
