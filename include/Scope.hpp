#ifndef SCOPE_H
#define SCOPE_H

#include "Token.hpp"
#include <cstddef>
#include <map>
#include <string>
#include <vector>

using size_t = std::size_t;

enum class SymbolKind { VARIABLE, FUNCTION };

enum class TypeKindE {
  // Types
  INT,
  CHAR,
  POINTER,
  VOID,
  NULLP
};

struct TypeKind {
  TypeKindE tk;
  TypeKindE to = TypeKindE::NULLP;
};

struct Symbol {
  SymbolKind kind;
  TypeKind type;

  std::vector<TypeKind> params;
};

int getRank(TypeKind tk);
TypeKind TokToType(TokenType tk);

TypeKind getTypeStruct(TypeKindE tkE);
TypeKind getTypeStruct(TypeKindE tkE, TypeKindE toE);

class Scope {
public:
  std::map<std::string, Symbol> symTable;

  void addRow(std::string name, TokenType tokentype, SymbolKind symKind);
  void addRow(std::string name, TypeKind type, SymbolKind symKind);
  bool search(std::string name);

  void addParam(std::string name, TypeKind type);
  size_t getNumParams(std::string name);
  TypeKind getSymType(std::string name);
  SymbolKind getSymKind(std::string name);
  std::vector<TypeKind> getParams(std::string name);
};

#endif
