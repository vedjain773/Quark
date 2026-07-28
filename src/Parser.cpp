#include "Parser.hpp"
#include "Error.hpp"
#include "Program.hpp"
#include "Visitor.hpp"
#include <iostream>

BinOpPrec getBinPrecedence(Operators Op) {
    switch (Op) {
        case Operators::MULT:
        case Operators::DIVIDE:
        case Operators::MODULUS:
            return FACTOR;
            break;

        case Operators::PLUS:
        case Operators::MINUS:
            return TERM;
            break;

        case Operators::GREATER:
        case Operators::GREATER_EQUALS:
        case Operators::LESS:
        case Operators::LESS_EQUALS:
            return COMP;
            break;

        case Operators::EQUALS:
        case Operators::NOT_EQUALS:
            return COMP_EQL;
            break;

        case Operators::AND:
            return LAND;
            break;

        case Operators::OR:
            return LOR;
            break;

        default:
            return MISC;
    }
}

bool isPostFixOp(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::LEFT_ROUND:
        case TokenType::LEFT_SQUARE:
        case TokenType::DOT:
        case TokenType::ARROW:
            return true;
            break;

        default:
            return false;
    }
}

bool isAssignOp(TokenType tokenType) {
    switch (tokenType) {
        case TokenType::EQUALS:
        case TokenType::PLUS_EQUALS:
        case TokenType::MINUS_EQUALS:
        case TokenType::ASTERISK_EQUALS:
        case TokenType::SLASH_EQUALS:
        case TokenType::MODULUS_EQUALS:
            return true;

        default:
            return false;
    }
}

std::tuple<TypeKind *, std::string> Parser::ParseTypePrefix() {
    std::string typeName = "";

    if (peekCurr().tokentype == TokenType::STRUCT) {
        typeName += peekCurr().lexeme + " ";
        getNextToken();
    }

    typeName += peekCurr().lexeme;

    getNextToken();

    TypeKind *typek = getType(typeName);

    while (peekCurr().tokentype == TokenType::ASTERISK) {
        typeName += '*';
        typek = getType(typeName);

        getNextToken();
    }

    return std::make_tuple(typek, typeName);
}

TypeKind *Parser::ParseTypeSuffix(TypeKind *typek, std::string typeName) {
    TypeKind *suffixType = typek;

    while (peekCurr().tokentype == TokenType::LEFT_SQUARE) {
        getNextToken();

        auto iExpr = ParseIntExpr();
        IntExpr *intExpr = static_cast<IntExpr *>(iExpr.get());

        if (peekCurr().tokentype != TokenType::RIGHT_SQUARE) {
            expect(peekCurr(), "Expected ']'");
        }

        getNextToken();
        suffixType = getArrType(typeName, intExpr->Val);
        typeName = suffixType->name;
    }

    return suffixType;
}

TypeKind *Parser::ParseType() {
    auto [prefixType, typeName] = ParseTypePrefix();
    return ParseTypeSuffix(prefixType, typeName);
}

std::tuple<TypeKind *, std::string, int, int> Parser::getTypeNamePair() {
    auto [typek, typeName] = ParseTypePrefix();

    if (peekCurr().tokentype != TokenType::IDENTIFIER) {
        expect(peekCurr(), "Expected identifier");
    }

    std::string varname = peekCurr().lexeme;

    int tline = peekCurr().line;
    int tcol = peekCurr().column;

    getNextToken();

    TypeKind *suffixType = ParseTypeSuffix(typek, typeName);

    return std::make_tuple(suffixType, varname, tline, tcol);
};

void Parser::expect(const Token &token, std::string msg) {
    Error error(token.line, token.column, msg);
    numOfErrors += 1;
    advToSyncPoint();
}

void Parser::expect(int line, int col, std::string msg) {
    Error error(line, col, msg);
    numOfErrors += 1;
    advToSyncPoint();
}

void Parser::advToSyncPoint() {
    auto isSyncPoint = [](const Token &token) {
        switch (token.tokentype) {
            case TokenType::SEMICOLON:
            case TokenType::LEFT_CURLY:
            case TokenType::RIGHT_CURLY:
            case TokenType::INT:
            case TokenType::CHAR:
            case TokenType::STRUCT:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::END_OF_FILE:
                return true;

            default:
                return false;
        }
    };

    while (!isSyncPoint(peekCurr()))
        getNextToken();
}

Parser::Parser(std::vector<Token> tokenlist)
    : TokenList(tokenlist), current(0) {}

Token Parser::getNextToken() {
    if (current < TokenList.size()) {
        return TokenList[current++];
    } else {
        return Token(TokenType::END_OF_FILE, "", 0, 0);
    }
}

Token Parser::peekCurr() { return TokenList[current]; }

Token Parser::peekNext() { return TokenList[current + 1]; }

Token Parser::peekPrev() { return TokenList[current - 1]; }

Token Parser::peekAhead(int n) { return TokenList[current + n]; }

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

    auto Result = ParseExpr();

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        expect(peekCurr(), "Missing ')' after expression");
        return nullptr;
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
            expect(peekCurr(), "Expected expression");
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
                    Operators::PLUS, std::move(Prim), std::move(inner), line,
                    column);

                auto Result = std::make_unique<DerefExpr>(std::move(binExpr),
                                                          line, column);

                getNextToken();

                Prim = std::move(Result);
            } break;

            case TokenType::DOT: {
                getNextToken();

                std::string fieldName = peekCurr().lexeme;

                auto Result = std::make_unique<MemberAccessExpr>(
                    std::move(Prim), fieldName, line, column);

                getNextToken();

                Prim = std::move(Result);
            } break;

            case TokenType::ARROW: {
                getNextToken();

                std::string fieldName = peekCurr().lexeme;

                auto deref =
                    std::make_unique<DerefExpr>(std::move(Prim), line, column);

                auto Result = std::make_unique<MemberAccessExpr>(
                    std::move(deref), fieldName, line, column);

                getNextToken();

                Prim = std::move(Result);
            } break;

            default: {
                advToSyncPoint();
                return nullptr;
            }
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

std::unique_ptr<Expression> Parser::ParseSizeOfExpr() {
    int tline = peekCurr().line;
    int tcol = peekCurr().column;

    getNextToken();

    if (!isTypeStarter(peekNext().tokentype)) {
        auto parenExpr = ParseParenExpr();
        auto Result =
            std::make_unique<SizeOfExpr>(std::move(parenExpr), tline, tcol);

        return Result;
    } else {
        // consume '('
        getNextToken();

        TypeKind *typek = ParseType();

        // consume ')'
        getNextToken();

        auto Result = std::make_unique<SizeOfExpr>(typek, tline, tcol);
        return Result;
    }
}

std::unique_ptr<Expression> Parser::ParseUnaryExpr() {
    switch (peekCurr().tokentype) {
        case TokenType::BANG:
        case TokenType::MINUS: {
            Operators oper = getOp(peekCurr().lexeme);

            int tline = peekCurr().line;
            int tcol = peekCurr().column;

            getNextToken();

            auto Result = std::make_unique<UnaryExpr>(oper, ParseUnaryExpr(),
                                                      tline, tcol);
            return Result;
        } break;

        case TokenType::ASTERISK: {
            return ParseDerefExpr();
        } break;

        case TokenType::AMPERSAND: {
            return ParseAddressExpr();
        } break;

        case TokenType::SIZEOF: {
            return ParseSizeOfExpr();
        } break;

        default:
            return ParsePostFixExpr();
    }
}

std::unique_ptr<Expression> Parser::ParseBinExpr(BinOpPrec level) {
    auto parseOperand = [&](BinOpPrec level) {
        if (level == FACTOR) {
            return ParseUnaryExpr();
        } else {
            BinOpPrec next = static_cast<BinOpPrec>(level - 1);
            return ParseBinExpr(next);
        }
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
    auto lhs = ParseBinExpr(LOR);

    if (isAssignOp(peekCurr().tokentype)) {
        Operators Op = getOp(peekCurr().lexeme);
        int tline = peekCurr().line;
        int tcol = peekCurr().column;

        getNextToken();
        auto rhs = ParseAssignExpr();

        if (Op == Operators::ASSIGN) {
            auto Result = std::make_unique<AssignExpr>(
                std::move(lhs), std::move(rhs), tline, tcol);
            return Result;
        } else {
            auto Result = std::make_unique<CompAssignExpr>(
                Op, std::move(lhs), std::move(rhs), tline, tcol);
            return Result;
        }
    } else {
        return lhs;
    }
}

std::unique_ptr<Expression> Parser::ParseExpr() { return ParseAssignExpr(); }

std::unique_ptr<Statement> Parser::ParseExprStmt() {
    std::unique_ptr<Expression> expr = ParseExpr();

    if (peekCurr().tokentype != TokenType::SEMICOLON) {
        expect(peekCurr(), "Missing ';' after expression");
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
            expect(peekCurr(), "Expected '}'");
            return nullptr;
        }
    }

    getNextToken();
    return Result;
}

std::unique_ptr<Statement> Parser::ParseIfStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        expect(peekCurr(), "Expected '('");
        return nullptr;
    }
    getNextToken();
    
    auto condn = ParseExpr();

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        expect(peekCurr(), "Missing ')'");
        return nullptr;
    }
    getNextToken();

    auto ifbody = ParseStmt();

    auto elsestmt = ParseElseStmt();
    auto Result = std::make_unique<IfStmt>(std::move(condn), std::move(ifbody),
                                           std::move(elsestmt));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseElseStmt() {
    if (peekCurr().tokentype != TokenType::ELSE)
        return nullptr;

    getNextToken();

    auto elsebody = ParseStmt();
    auto Result = std::make_unique<ElseStmt>(std::move(elsebody));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseWhileStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        expect(peekCurr(), "Expected '('");
        return nullptr;
    }

    getNextToken();
    auto condn = ParseExpr();

    if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
        expect(peekCurr(), "Missing ')'");
        return nullptr;
    }
    getNextToken();

    auto whilebody = ParseStmt();
    auto Result =
        std::make_unique<WhileStmt>(std::move(condn), std::move(whilebody));
    return Result;
}

std::unique_ptr<Statement> Parser::ParseForStmt() {
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        expect(peekCurr(), "Expected '('");
        return nullptr;
    }
    getNextToken();

    std::unique_ptr<Statement> init;

    if (isTypeStarter(peekCurr().tokentype)) {
        init = ParseDeclStmt();
    } else if (peekCurr().tokentype == TokenType::SEMICOLON) {
        init = std::make_unique<EmptyStmt>();
        getNextToken();
    } else {
        init = ParseExprStmt();
    }

    std::unique_ptr<Expression> condn, iter;
    
    if (peekCurr().tokentype == TokenType::SEMICOLON) {
        condn = std::make_unique<EmptyExpr>();
    } else {
        condn = ParseExpr();

        if (peekCurr().tokentype != TokenType::SEMICOLON) {
            expect(peekCurr(), "Expected ';'");
            return nullptr;
        }
    }
    getNextToken();

    if (peekCurr().tokentype == TokenType::RIGHT_ROUND) {
        iter = std::make_unique<EmptyExpr>();
    } else {
        iter = ParseExpr();

        if (peekCurr().tokentype != TokenType::RIGHT_ROUND) {
            expect(peekCurr(), "Expected ')'");
            return nullptr;
        }
    }
    getNextToken();

    auto body = ParseStmt();
    auto Result = std::make_unique<ForStmt>(std::move(init), std::move(condn),
                                            std::move(iter), std::move(body));

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
    auto [typek, varname, tline, tcol] = getTypeNamePair();

    int lastTokenLine, lastTokenCol;
    std::unique_ptr<Expression> expr;

    if (peekCurr().tokentype == TokenType::EQUALS) {
        getNextToken();
        expr = ParseExpr();

        lastTokenLine = expr->line;
        lastTokenCol = expr->column;
    } else {
        lastTokenLine = peekCurr().line;
        lastTokenCol = peekCurr().column;

        expr = nullptr;
    }

    auto Result = std::make_unique<DeclStmt>(typek, varname, std::move(expr),
                                             tline, tcol);

    if (peekCurr().tokentype != TokenType::SEMICOLON) {
        expect(lastTokenLine, lastTokenCol, "Missing ';' after declaration");
        return nullptr;
    }

    getNextToken();
    return Result;
}

std::unique_ptr<StructField> Parser::ParseStructField() {
    auto [typek, fieldName, tline, tcol] = getTypeNamePair();

    return std::make_unique<StructField>(typek, fieldName, tline, tcol);
}

std::unique_ptr<StructDecl> Parser::ParseStructDecl() {
    int line = peekCurr().line;
    int column = peekCurr().column;
    getNextToken();

    std::string tag = peekCurr().lexeme;

    TypeKind *structType = createStructType(tag);

    auto Result = std::make_unique<StructDecl>(tag, line, column);
    getNextToken();

    if (peekCurr().tokentype != TokenType::LEFT_CURLY) {
        expect(peekCurr(), "Expected '{'");
        return nullptr;
    }

    getNextToken();

    int offset = 0;

    while (peekCurr().tokentype != TokenType::RIGHT_CURLY) {
        auto structField = ParseStructField();

        int fieldAlign = structField->type->align;
        int fieldSize = structField->type->size;

        if (offset % fieldAlign != 0)
            offset += fieldAlign - offset % fieldAlign;

        offset += fieldSize;

        Result->addField(std::move(structField));
        getNextToken();
    }

    if (offset > 0)
        structType->size = offset;

    if (peekCurr().tokentype != TokenType::RIGHT_CURLY) {
        expect(peekCurr(), "Expected '}'");
        return nullptr;
    }

    // Consume }
    getNextToken();

    if (peekCurr().tokentype != TokenType::SEMICOLON) {
        expect(peekCurr(), "Missing ';' after struct declaration");
        return Result;
    }

    // Consume ;
    getNextToken();
    return Result;
}

std::unique_ptr<Parameter> Parser::ParseParameter() {
    auto [typek, name, tline, tcol] = getTypeNamePair();

    auto Result = std::make_unique<Parameter>(typek, name);
    return Result;
}

std::unique_ptr<Prototype> Parser::ParsePrototype() {
    auto [typek, name, tline, tcol] = getTypeNamePair();

    if (peekCurr().tokentype != TokenType::LEFT_ROUND) {
        expect(peekCurr(), "Expected '('");
        return nullptr;
    }
    getNextToken();

    auto Result = std::make_unique<Prototype>(typek, name, tline, tcol);
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
            expect(peekCurr(), "Unexpected '}'");
            return nullptr;
        } break;

        case TokenType::RIGHT_ROUND: {
            expect(peekCurr(), "Unexpected ')'");
            return nullptr;
        } break;

        case TokenType::LEFT_CURLY: {
            return ParseBlockStmt();
        } break;

        case TokenType::INT:
        case TokenType::CHAR:
        case TokenType::UINT8: {
            return ParseDeclStmt();
        } break;

        case TokenType::STRUCT: {
            if (peekAhead(2).tokentype == TokenType::LEFT_CURLY)
                return ParseStructDecl();
            else
                return ParseDeclStmt();
        } break;

        case TokenType::IF: {
            return ParseIfStmt();
        } break;

        case TokenType::WHILE: {
            return ParseWhileStmt();
        } break;

        case TokenType::FOR: {
            return ParseForStmt();
        } break;

        case TokenType::BREAK: {
            auto Result =
                std::make_unique<BreakStmt>(peekCurr().line, peekCurr().column);

            // consume break
            getNextToken();

            // consume ;
            getNextToken();

            return Result;
        } break;

        case TokenType::CONTINUE: {
            auto Result = std::make_unique<ContinueStmt>(peekCurr().line,
                                                         peekCurr().column);

            // consume break
            getNextToken();

            // consume ;
            getNextToken();

            return Result;
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

        if (peekAhead(2).tokentype == TokenType::LEFT_CURLY) {
            auto edecl = ParseStructDecl();
            program->add(std::move(edecl));
        } else {
            auto edecl = ParseFuncDef();
            program->add(std::move(edecl));
        }
    }

    return program;
}
