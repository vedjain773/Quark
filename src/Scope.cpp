#include "Scope.hpp"

TypeKind int_tk = {"int", nullptr};
TypeKind char_tk = {"char", nullptr};
TypeKind void_tk = {"void", nullptr};
TypeKind null_tk = {"null", nullptr};

std::unordered_map<std::string, TypeKind*> typeTable = {
  {"int", &int_tk},
  {"char", &char_tk},
  {"void", &void_tk},
  {"null", &null_tk}
};

TypeKind* TokToType(TokenType tk) {
  switch (tk) {
  case TokenType::INT: {
    return typeTable["int"];
  } break;

  case TokenType::CHAR: {
    return typeTable["char"];
  } break;

  default:
    return typeTable["void"];

  }
}

TypeKind* getType(std::string typeName) {
  int size = typeName.size();

  if (typeTable.count(typeName) != 0) {
    return typeTable[typeName];
  } else if (typeName[size - 1] == '*') {
    TypeKind* base = typeTable[typeName.substr(0, size - 1)];

    TypeKind newType = {typeName, base};
    typeTable[typeName] = &newType;
    return typeTable[typeName];
  }

  return typeTable["null"];
}

bool isPointerType(TypeKind* typek) {
  int size = typek->name.size();
  return typek->name[size - 1] == '*';
}

void Scope::addRow(std::string name, TokenType tokentype, SymbolKind symKind) {
  Symbol symbol;
  symbol.type = TokToType(tokentype);
  symbol.kind = symKind;

  symTable.insert({name, symbol});
}

void Scope::addRow(std::string name, TypeKind* type, SymbolKind symKind) {
  Symbol symbol;
  symbol.type = type;
  symbol.kind = symKind;

  symTable.insert({name, symbol});
}

bool Scope::search(std::string name) { return symTable.count(name); }

void Scope::addParam(std::string name, TypeKind* type) {
  Symbol &sym = symTable[name];
  sym.params.push_back(type);
}

size_t Scope::getNumParams(std::string name) {
  Symbol &sym = symTable[name];
  return sym.params.size();
}

TypeKind* Scope::getSymType(std::string name) {
  Symbol &sym = symTable[name];
  return sym.type;
}

SymbolKind Scope::getSymKind(std::string name) {
  Symbol &sym = symTable[name];
  return sym.kind;
}

std::vector<TypeKind*> Scope::getParams(std::string name) {
  Symbol &sym = symTable[name];
  return sym.params;
}
