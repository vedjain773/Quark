#include "utils/Scope.hpp"
#include <iostream>

std::unordered_map<std::string, std::unique_ptr<TypeKind>> typeTable = [] {
    std::unordered_map<std::string, std::unique_ptr<TypeKind>> m;

    m.emplace("int", std::make_unique<TypeKind>(
                         TypeKind{TypeEnum::BUILTIN, "int", 4, 4, nullptr}));
    m.emplace("uint8_t", std::make_unique<TypeKind>(TypeKind{
                             TypeEnum::BUILTIN, "uint8_t", 1, 1, nullptr}));
    m.emplace("uint16_t", std::make_unique<TypeKind>(TypeKind{
                              TypeEnum::BUILTIN, "uint16_t", 2, 2, nullptr}));
    m.emplace("char", std::make_unique<TypeKind>(
                          TypeKind{TypeEnum::BUILTIN, "char", 1, 1, nullptr}));
    m.emplace("void", std::make_unique<TypeKind>(
                          TypeKind{TypeEnum::BUILTIN, "void", 0, 0, nullptr}));
    m.emplace("null", std::make_unique<TypeKind>(
                          TypeKind{TypeEnum::BUILTIN, "null", 0, 0, nullptr}));
    m.emplace("error", std::make_unique<TypeKind>(
                           TypeKind{TypeEnum::ERROR, "error", 0, 0, nullptr}));

    return m;
}();

TypeKind *TokToType(TokenType tk) {
    switch (tk) {
        case TokenType::INT: {
            return typeTable["int"].get();
        } break;

        case TokenType::UINT8: {
            return typeTable["uint8_t"].get();
        } break;

        case TokenType::UINT16: {
            return typeTable["uint16_t"].get();
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

        std::unique_ptr<TypeKind> newType = std::make_unique<TypeKind>(
            TypeKind{TypeEnum::POINTER, typeName, 8, 8, base});

        TypeKind *newType_raw = newType.get();

        typeTable[typeName] = std::move(newType);
        return newType_raw;
    }

    return typeTable["null"].get();
}

TypeKind *getArrType(std::string typeName, int numOfElements) {
    TypeKind *base = getType(typeName);
    size_t baseSize = base->size;

    size_t arrSize = numOfElements * baseSize;

    std::string newTypeName = typeName + "[]";

    std::unique_ptr<TypeKind> newType = std::make_unique<TypeKind>(
        TypeKind{TypeEnum::ARRAY, newTypeName, arrSize, base->align, base});

    TypeKind *newType_raw = newType.get();

    typeTable[newTypeName] = std::move(newType);
    return newType_raw;
}

TypeKind *createStructType(std::string tag) {
    std::string typeName = "struct ";
    typeName += tag;

    std::unique_ptr<TypeKind> newType = std::make_unique<TypeKind>(
        TypeKind{TypeEnum::STRUCT, typeName, 1, 8, nullptr});

    typeTable[typeName] = std::move(newType);
    return typeTable[typeName].get();
}

bool isPointerType(TypeKind *typek) {
    return typek->type == TypeEnum::POINTER;
}

bool isArrayType(TypeKind *typek) {
    return typek->type == TypeEnum::ARRAY;
}

bool isStructType(TypeKind *typek) {
    return typek->type == TypeEnum::STRUCT;
}

bool isErrorType(TypeKind *typek) {
    return typek->type == TypeEnum::ERROR;
}

int getNumElements(TypeKind *typek) {
    int arrSize = typek->size;
    int elementSize = typek->to->size;

    return (arrSize / elementSize);
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

bool Scope::search(std::string name) {
    return symTable.count(name);
}

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
