#include "Scope.hpp"

int getRank(TypeKind tk)  {
    switch (tk.tk) {
        case TypeKindE::INT: {
            return 2;
        }
        break;

        case TypeKindE::CHAR: {
            return 1;
        }
        break;

        default: {
            return 0;
        }
    }
}

TypeKind TokToType(TokenType tk) {
    TypeKind typek;
    switch (tk) {
        case TokenType::INT: {
            typek.tk = TypeKindE::INT;
        }
        break;

        case TokenType::CHAR: {
            typek.tk = TypeKindE::CHAR;
        }
        break;

        default: {
            typek.tk = TypeKindE::VOID;
        }
    }

    return typek;
}

TypeKind getTypeStruct(TypeKindE tkE) {
    TypeKind typeK;
    typeK.tk = tkE;

    return typeK;
}

TypeKind getTypeStruct(TypeKindE tkE, TypeKindE toE) {
    TypeKind typeK;
    typeK.tk = tkE;
    typeK.to = toE;

    return typeK;
}

void Scope::addRow(std::string name, TokenType tokentype, SymbolKind symKind) {
    Symbol symbol;
    symbol.type = TokToType(tokentype);
    symbol.kind = symKind;

    symTable.insert({name, symbol});
}

void Scope::addRow(std::string name, TypeKind type, SymbolKind symKind) {
    Symbol symbol;
    symbol.type = type;
    symbol.kind = symKind;

    symTable.insert({name, symbol});
}

bool Scope::search(std::string name) {
    return symTable.count(name);
}

void Scope::addParam(std::string name, TypeKind type) {
    Symbol &sym = symTable[name];
    sym.params.push_back(type);
}

int Scope::getNumParams(std::string name) {
    Symbol &sym = symTable[name];
    return sym.params.size();
}

TypeKind Scope::getSymType(std::string name) {
    Symbol &sym = symTable[name];
    return sym.type;
}

SymbolKind Scope::getSymKind(std::string name) {
    Symbol &sym = symTable[name];
    return sym.kind;
}

std::vector<TypeKind> Scope::getParams(std::string name) {
    Symbol &sym = symTable[name];
    return sym.params;
}
