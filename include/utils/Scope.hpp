#ifndef SCOPE_H
#define SCOPE_H

#include "frontend/scanner/Token.hpp"
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <vector>

using size_t = std::size_t;

enum class SymbolKind { VARIABLE, FUNCTION };

enum class TypeEnum {
    // Primitives
    BUILTIN,

    // Aggregate
    POINTER,
    ARRAY,
    STRUCT,

    // Error
    ERROR
};

struct TypeKind {
    TypeEnum type;

    std::string name;
    size_t size;
    size_t align;

    TypeKind *to = nullptr;

    struct Field {
        TypeKind *fType;
        std::string name;
    };

    std::vector<Field> fields;
};

struct Symbol {
    SymbolKind kind;
    TypeKind *type;

    std::vector<TypeKind *> params;
};

TypeKind *TokToType(TokenType tk);

extern std::unordered_map<std::string, std::unique_ptr<TypeKind>> typeTable;

TypeKind *getType(std::string typeName);
TypeKind *getArrType(std::string typeName, int numOfElements);
TypeKind *createStructType(std::string tag);

bool isPointerType(TypeKind *typek);
bool isArrayType(TypeKind *typek);
bool isStructType(TypeKind *typek);
bool isErrorType(TypeKind *typek);

int getNumElements(TypeKind *typek);

class Scope {
  public:
    std::map<std::string, Symbol> symTable;

    void addRow(std::string name, TokenType tokentype, SymbolKind symKind);
    void addRow(std::string name, TypeKind *type, SymbolKind symKind);
    bool search(std::string name);

    void addParam(std::string name, TypeKind *type);
    size_t getNumParams(std::string name);
    TypeKind *getSymType(std::string name);
    SymbolKind getSymKind(std::string name);
    std::vector<TypeKind *> getParams(std::string name);
};

#endif
