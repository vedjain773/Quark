#include "Scope.hpp"
#include <iostream>

std::unordered_map<std::string, std::unique_ptr<TypeKind>> typeTable = [] {
    std::unordered_map<std::string, std::unique_ptr<TypeKind>> m;

    m.emplace("int", std::make_unique<TypeKind>(TypeKind{"int", nullptr}));
    m.emplace("char", std::make_unique<TypeKind>(TypeKind{"char", nullptr}));
    m.emplace("void", std::make_unique<TypeKind>(TypeKind{"void", nullptr}));
    m.emplace("null", std::make_unique<TypeKind>(TypeKind{"null", nullptr}));

    return m;
}();

TypeKind *TokToType(TokenType tk) {
    switch (tk) {
    case TokenType::INT: {
        return typeTable["int"].get();
    } break;

    case TokenType::CHAR: {
        return typeTable["char"].get();
    } break;

    default:
        return typeTable["void"].get();
    }
}

TypeKind *getType(std::string typeName) {
    int size = typeName.size();

    if (typeTable.count(typeName) != 0) {
        return typeTable[typeName].get();
    } else if (typeName[size - 1] == '*') {
        TypeKind *base = typeTable[typeName.substr(0, size - 1)].get();

        std::unique_ptr<TypeKind> newType =
            std::make_unique<TypeKind>(TypeKind{typeName, base});

        TypeKind *newType_raw = newType.get();

        typeTable[typeName] = std::move(newType);
        return newType_raw;
    }

    return typeTable["null"].get();
}

bool isPointerType(TypeKind *typek) {
    int size = typek->name.size();
    char lastChar = typek->name[size - 1];

    return lastChar == '*';
}

void Scope::addRow(std::string name, TokenType tokentype, SymbolKind symKind) {
    Symbol symbol;
    symbol.type = TokToType(tokentype);
    symbol.kind = symKind;

    symTable.insert({name, symbol});
}

void Scope::addRow(std::string name, TypeKind *type, SymbolKind symKind) {
    Symbol symbol;
    symbol.type = type;
    symbol.kind = symKind;

    symTable.insert({name, symbol});
}

bool Scope::search(std::string name) { return symTable.count(name); }

void Scope::addParam(std::string name, TypeKind *type) {
    Symbol &sym = symTable[name];
    sym.params.push_back(type);
}

size_t Scope::getNumParams(std::string name) {
    Symbol &sym = symTable[name];
    return sym.params.size();
}

TypeKind *Scope::getSymType(std::string name) {
    Symbol &sym = symTable[name];
    return sym.type;
}

SymbolKind Scope::getSymKind(std::string name) {
    Symbol &sym = symTable[name];
    return sym.kind;
}

std::vector<TypeKind *> Scope::getParams(std::string name) {
    Symbol &sym = symTable[name];
    return sym.params;
}
