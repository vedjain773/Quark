#ifndef SCOPE_H
#define SCOPE_H

#include <map>
#include <string>
#include <vector>
#include "Token.hpp"

enum class SymbolKind {
    VARIABLE,
    FUNCTION
};

enum class TypeKindE {
    //Types
    INT, CHAR, VOID
};

struct TypeKind {
    TypeKindE tk;
    TypeKind* to = nullptr;
};

struct Symbol {
    SymbolKind kind;
    TypeKind type;

    std::vector<TypeKind> params;
};

int getRank(TypeKind tk);
TypeKind TokToType(TokenType tk);
TypeKind getTypeStruct(TypeKindE tkE);

class Scope {
    public:
    std::map<std::string, Symbol> symTable;

    void addRow(std::string name, TokenType tokentype, SymbolKind symKind);
    void addRow(std::string name, TypeKind type, SymbolKind symKind);
    bool search(std::string name);

    void addParam(std::string name, TypeKind type);
    int getNumParams(std::string name);
    TypeKind getSymType(std::string name);
    SymbolKind getSymKind(std::string name);
    std::vector<TypeKind> getParams(std::string name);
};

#endif
