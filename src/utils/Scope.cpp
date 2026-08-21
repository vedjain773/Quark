#include "utils/Scope.hpp"
#include <iostream>

std::unordered_map<std::string, std::unique_ptr<TypeKind>> typeTable = [] {
    std::unordered_map<std::string, std::unique_ptr<TypeKind>> m;

    m.emplace("int", std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "int", 4, 4, nullptr}));
    m.emplace("uint8_t",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "uint8_t", 1, 1, nullptr}));
    m.emplace("uint16_t",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "uint16_t", 2, 2, nullptr}));
    m.emplace("char",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "char", 1, 1, nullptr}));
    m.emplace("void",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "void", 0, 0, nullptr}));
    m.emplace("null",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::BUILTIN, "null", 0, 0, nullptr}));
    m.emplace("error",
              std::make_unique<TypeKind>(TypeKind{TypeEnum::ERROR, "error", 0, 0, nullptr}));

    return m;
}();

TypeKind *getType(const std::string &typeName) {
    int size = typeName.size();

    if (typeTable.count(typeName) != 0) {
        return typeTable[typeName].get();
    } else if (typeName[size - 1] == '*') {
        TypeKind *base = typeTable[typeName.substr(0, size - 1)].get();

        std::unique_ptr<TypeKind> newType =
            std::make_unique<TypeKind>(TypeKind{TypeEnum::POINTER, typeName, 8, 8, base});

        TypeKind *newType_raw = newType.get();

        typeTable[typeName] = std::move(newType);
        return newType_raw;
    }

    return typeTable["null"].get();
}

TypeKind *getArrType(const std::string &typeName, int numOfElements) {
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

TypeKind *createStructType(const std::string &tag) {
    std::string typeName = "struct ";
    typeName += tag;

    std::unique_ptr<TypeKind> newType =
        std::make_unique<TypeKind>(TypeKind{TypeEnum::STRUCT, typeName, 1, 8, nullptr});

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

void Scope::addRow(const std::string &name, TypeKind *type, SymbolKind symKind) {
    Symbol symbol;
    symbol.type = type;
    symbol.kind = symKind;

    symTable.insert({name, symbol});
}

bool Scope::search(const std::string &name) {
    return symTable.count(name);
}

void Scope::addParam(const std::string &name, TypeKind *type) {
    Symbol &sym = symTable[name];
    sym.params.push_back(type);
}

size_t Scope::getNumParams(const std::string &name) {
    Symbol &sym = symTable[name];
    return sym.params.size();
}

TypeKind *Scope::getSymType(const std::string &name) {
    Symbol &sym = symTable[name];
    return sym.type;
}

SymbolKind Scope::getSymKind(const std::string &name) {
    Symbol &sym = symTable[name];
    return sym.kind;
}

std::vector<TypeKind *> Scope::getParams(const std::string &name) {
    Symbol &sym = symTable[name];
    return sym.params;
}
