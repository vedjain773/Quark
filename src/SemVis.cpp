#include "Error.hpp"
#include "Expression.hpp"
#include "ExternalDecl.hpp"
#include "Function.hpp"
#include "Program.hpp"
#include "Statement.hpp"
#include "Visitor.hpp"
#include <cstddef>
#include <iostream>

using size_t = std::size_t;

void SemanticVisitor::visitProgram(Program &program) {
    Scope globalScope;
    scopeVec.push_back(globalScope);

    for (size_t i = 0; i < program.root.size(); i++) {
        ExternalDecl *edecl = (program.root[i]).get();

        if (edecl == nullptr)
            continue;

        edecl->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitParameter(Parameter &parameter) {
    scopeVec[scopeVec.size() - 1].addRow(parameter.name, parameter.type,
                                         SymbolKind::VARIABLE);
}

void SemanticVisitor::visitPrototype(Prototype &prototype) {

    if (scopeVec[0].search(prototype.funcName)) {
        Error error(prototype.line, prototype.column,
                    prototype.funcName + " is already declared");
        numOfErrors += 1;
    } else {
        scopeVec[0].addRow(prototype.funcName, prototype.retType,
                           SymbolKind::FUNCTION);
        currFuncRetType = prototype.retType;
    }

    for (size_t i = 0; i < prototype.paramList.size(); i++) {
        Parameter *param = (prototype.paramList[i]).get();
        param->accept(*this);
        scopeVec[0].addParam(prototype.funcName, param->type);
    }
}

void SemanticVisitor::visitFuncDef(FuncDef &funcdef) {
    Scope funcScope;
    scopeVec.push_back(funcScope);

    Prototype *proto = (funcdef.prototype).get();
    BlockStmt *body = (funcdef.funcBody).get();

    proto->accept(*this);

    for (size_t i = 0; i < body->statements.size(); i++) {
        Statement *statmt = (body->statements[i]).get();
        statmt->accept(*this);
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitEmptyStmt(EmptyStmt &emptystmt) {
    // no checks
}

void SemanticVisitor::visitBlockStmt(BlockStmt &blockstmt) {
    Scope locScope;
    scopeVec.push_back(locScope);

    bool isTerm = false;

    for (size_t i = 0; i < blockstmt.statements.size(); i++) {
        Statement *stmt = (blockstmt.statements[i]).get();

        if (isTerm) {
            Warning warning(stmt->line, stmt->column, "Statement is unreachable");
        } 

        stmt->accept(*this);

        if (stmt->isTerminator())
            isTerm = true;
    }

    scopeVec.pop_back();
}

void SemanticVisitor::visitDeclStmt(DeclStmt &declstmt) {
    if (scopeVec[scopeVec.size() - 1].search(declstmt.name)) {
        Error error(declstmt.line, declstmt.column,
                    declstmt.name + " is already declared");
        numOfErrors += 1;
    } else {
        scopeVec[scopeVec.size() - 1].addRow(declstmt.name, declstmt.type,
                                             SymbolKind::VARIABLE);
    }

    Expression *expr = (declstmt.expression).get();

    if (expr != nullptr) {
        expr->accept(*this);

        if (expr->infType == getType("void")) {
            Error error(declstmt.line, declstmt.column,
                        "Variables cannot be of type: void");
            numOfErrors += 1;
        }

        if (expr->infType != declstmt.type) {
            auto castexpr = std::make_unique<CastExpr>(
                std::move(declstmt.expression), expr->infType, declstmt.type);

            Expression *cexpr = castexpr.get();
            cexpr->accept(*this);

            declstmt.expression = std::move(castexpr);
        }

        declstmt.expression->infType = declstmt.type;
    }
}

void SemanticVisitor::visitStructDecl(StructDecl &structdecl) {
    std::string typeName = "struct ";
    typeName += structdecl.tag;

    if (structdecl.fields.empty())
        return;

    TypeKind *typek = getType(typeName);
    typek->size = 0;

    int offset = 0;

    for (size_t i = 0; i < structdecl.fields.size(); i++) {
        StructField *structField = (structdecl.fields[i]).get();

        int fieldAlign = structField->type->align;
        int fieldSize = structField->type->size;

        if (offset % fieldAlign != 0)
            offset += fieldAlign - offset % fieldAlign;

        offset += fieldSize;

        typek->fields.push_back({structField->type, structField->fName});
    }

    if (offset > 0)
        typek->size = offset;
}

void SemanticVisitor::visitStructField(StructField &structfield) {
    // do nothing
}

void SemanticVisitor::visitIfStmt(IfStmt &ifstmt) {
    Expression *condn = (ifstmt.condition).get();
    Statement *ifbody = (ifstmt.body).get();
    Statement *elsestmt = (ifstmt.elseStmt).get();

    condn->accept(*this);

    if (condn->infType != getType("int")) {
        std::string typeName = condn->infType->name;
        Error error(ifstmt.line, ifstmt.column,
                    "Invalid (if) condition expression; Expected: int, Got: " +
                        typeName);
        numOfErrors += 1;
    }

    ifbody->accept(*this);

    if (elsestmt != nullptr) {
        elsestmt->accept(*this);
    }
}

void SemanticVisitor::visitElseStmt(ElseStmt &elsestmt) {
    Statement *elsebody = (elsestmt.body).get();

    elsebody->accept(*this);
}

void SemanticVisitor::visitWhileStmt(WhileStmt &whilestmt) {
    Expression *condn = (whilestmt.condition).get();
    Statement *whilebody = (whilestmt.body).get();

    condn->accept(*this);

    if (condn->infType != getType("int")) {
        std::string typeName = condn->infType->name;
        Error error(
            whilestmt.line, whilestmt.column,
            "Invalid (while) condition expression, Expected: int, Got: " +
                typeName);
        numOfErrors += 1;
    }
    
    insideLoop++;
    whilebody->accept(*this);
    insideLoop--;
}

void SemanticVisitor::visitBreakStmt(BreakStmt &breakstmt) {
    if (insideLoop <= 0) {
        Error error(breakstmt.line, breakstmt.column,
                "Break statements must be inside while loops");
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitContinueStmt(ContinueStmt &continuestmt) {
    if (insideLoop <= 0) {
        Error error(continuestmt.line, continuestmt.column,
                "Continue statements must be inside while loops");
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitReturnStmt(ReturnStmt &returnstmt) {
    Expression *retexpr = (returnstmt.retExpr).get();

    retexpr->accept(*this);

    if (isErrorType(retexpr->infType))
        return;

    if (retexpr->infType != currFuncRetType) {
        std::string retexprTypeName = retexpr->infType->name;
        std::string currRetTypeName = currFuncRetType->name;
        Error error(retexpr->line, retexpr->column,
                    "Return type (" + retexprTypeName +
                        ") does not match function signature (" +
                        currRetTypeName + ")");

        numOfErrors += 1;
    }
}

void SemanticVisitor::visitExprStmt(ExprStmt &exprstmt) {
    Expression *expr = (exprstmt.expression).get();

    expr->accept(*this);
}

void SemanticVisitor::visitMemberAccessExpr(MemberAccessExpr &memexpr) {
    Expression *expr = (memexpr.base).get();

    expr->accept(*this);

    if (!isStructType(expr->infType)) {
        std::cout << expr->infType->name << "\n";
        Error error(expr->line, expr->column,
                    "Base expression is not a struct");
        numOfErrors += 1;
        return;
    }

    bool found = false;
    int index = 0;
    for (size_t i = 0; i < expr->infType->fields.size(); i++) {
        if (expr->infType->fields[i].name == memexpr.fName) {
            found = true;
            index = i;
            break;
        }
    }

    if (!found) {
        std::string msg = "Struct has no field: ";
        msg += memexpr.fName;

        Error error(expr->line, expr->column, msg);
        numOfErrors += 1;
        return;
    }

    memexpr.infType = expr->infType->fields[index].fType;
}
void SemanticVisitor::visitEmptyExpr(EmptyExpr &emptyexpr) {
    emptyexpr.infType = getType("void");
}

void SemanticVisitor::visitAssignExpr(AssignExpr &assignexpr) {
    Expression *lExpr = (assignexpr.LHS).get();
    Expression *rExpr = (assignexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);
    
    if (!lExpr->isLValue()) {
        Error error(lExpr->line, lExpr->column, "Expression is not assignable");
        numOfErrors += 1;
        return;
    }

    if (rExpr->infType == getType("void")) {
        Error error(assignexpr.line, assignexpr.column,
                    "Assignment operand cannot be of Type: void");
        numOfErrors += 1;
        return;
    }

    if (lExpr->infType != rExpr->infType) {
        auto castexpr = std::make_unique<CastExpr>(std::move(assignexpr.RHS),
                                                   assignexpr.RHS->infType,
                                                   assignexpr.LHS->infType);

        Expression *cexpr = castexpr.get();
        cexpr->accept(*this);

        assignexpr.RHS = std::move(castexpr);
    }

    assignexpr.infType = assignexpr.LHS->infType;
}

void SemanticVisitor::visitCompAssignExpr(CompAssignExpr &compassignexpr) {
    Expression *lExpr = (compassignexpr.LHS).get();
    Expression *rExpr = (compassignexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);
    
    if (!lExpr->isLValue()) {
        Error error(lExpr->line, lExpr->column, "Expression is not assignable");
        numOfErrors += 1;
        return;
    }

    if (rExpr->infType == getType("void")) {
        Error error(compassignexpr.line, compassignexpr.column,
                    "Assignment operand cannot be of Type: void");
        numOfErrors += 1;
        return;
    }

    compassignexpr.infType = compassignexpr.LHS->infType;
}


void SemanticVisitor::visitBinaryExpr(BinaryExpr &binexpr) {
    Expression *lExpr = (binexpr.LHS).get();
    Expression *rExpr = (binexpr.RHS).get();

    lExpr->accept(*this);

    rExpr->accept(*this);

    if (lExpr->infType == getType("void")) {
        Error error(lExpr->line, lExpr->column,
                    "Binary Operand cannot be of type: void");
        numOfErrors += 1;
        return;
    }

    if (rExpr->infType == getType("void")) {
        Error error(rExpr->line, rExpr->column,
                    "Binary Operand cannot be of type: void");
        numOfErrors += 1;
        return;
    }

    bool isLPointerOrArray =
        isPointerType(lExpr->infType) || isArrayType(lExpr->infType);
    bool isRPointerOrArray =
        isPointerType(rExpr->infType) || isArrayType(rExpr->infType);

    if (lExpr->infType != rExpr->infType) {
        if (!isLPointerOrArray && !isRPointerOrArray) {
            auto castexpr = std::make_unique<CastExpr>(std::move(binexpr.RHS),
                                                       binexpr.RHS->infType,
                                                       binexpr.LHS->infType);

            Expression *cexpr = castexpr.get();
            cexpr->accept(*this);

            binexpr.RHS = std::move(castexpr);
        } else {
            handlePointerArithmetic(binexpr);
        }
    }

    binexpr.infType = binexpr.LHS->infType;
}

void SemanticVisitor::handlePointerArithmetic(BinaryExpr &binexpr) {
    Expression *lExpr = (binexpr.LHS).get();
    Expression *rExpr = (binexpr.RHS).get();

    bool isLPointerOrArray =
        isPointerType(lExpr->infType) || isArrayType(lExpr->infType);
    bool isRPointerOrArray =
        isPointerType(rExpr->infType) || isArrayType(rExpr->infType);

    if (isLPointerOrArray && isRPointerOrArray) {
        Error error(lExpr->line, lExpr->column,
                    "Pointer-Pointer operations are not supported");
        numOfErrors += 1;
        return;
    }

    if (isRPointerOrArray) {
        Error error(rExpr->line, rExpr->column,
                    "Pointers must be left operands");
        numOfErrors += 1;
        return;
    }

    binexpr.infType = lExpr->infType;
    return;
}

void SemanticVisitor::visitSizeOfExpr(SizeOfExpr &sizeofexpr) {
    Expression *expr = (sizeofexpr.expr).get();
   
    if (expr != nullptr) {
        expr->accept(*this);
        sizeofexpr.argType = expr->infType;
    }

    sizeofexpr.infType = getType("int");
}

void SemanticVisitor::visitUnaryExpr(UnaryExpr &unaryexpr) {
    Expression *Operand = (unaryexpr.Operand).get();

    Operand->accept(*this);

    if (Operand->infType == getType("int")) {
        unaryexpr.infType = getType("int");
    } else {
        std::string typeName = Operand->infType->name;
        Error error(unaryexpr.line, unaryexpr.column,
                    "Operand must be of type: int, Got: " + typeName);
        numOfErrors += 1;
    }
}

void SemanticVisitor::visitDerefExpr(DerefExpr &derefexpr) {
    Expression *expr = (derefexpr.expr).get();

    expr->accept(*this);

    if (!isPointerType(expr->infType) && !isArrayType(expr->infType)) {
        std::string typeName = expr->infType->name;
        Error error(derefexpr.line, derefexpr.column,
                    "Expected: pointer, Got: " + typeName);
        numOfErrors += 1;
    }

    derefexpr.infType = expr->infType->to;
}

void SemanticVisitor::visitAddressExpr(AddressExpr &addressexpr) {
    Expression *expr = (addressexpr.expr).get();

    if (!expr->isLValue()) {
        Error error(addressexpr.line, addressexpr.column,
                    "Operand must be an lvalue");
        numOfErrors += 1;
    }

    expr->accept(*this);

    std::string typeName = expr->infType->name;
    addressexpr.infType = getType(typeName + '*');
}

void SemanticVisitor::visitCastExpr(CastExpr &castexpr) {
    if (isArrayType(castexpr.from)) {
        std::string message = "Cannot cast from type: ";
        message += castexpr.from->name + " to ";
        message += castexpr.to->name;

        Error error(castexpr.expr->line, castexpr.expr->column, message);

        numOfErrors += 1;
        return;
    }

    castexpr.infType = castexpr.to;
}

void SemanticVisitor::visitCallExpr(CallExpr &callexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(callexpr.callee)) {
            flag = true;

            if (scopeVec[i].getSymKind(callexpr.callee) ==
                SymbolKind::FUNCTION) {
                callexpr.infType = scopeVec[i].getSymType(callexpr.callee);
                break;
            } else {
                Error error(callexpr.line, callexpr.column,
                            callexpr.callee + " is not a callable function");
                numOfErrors += 1;
            }
        }
    }

    if (!flag) {
        Error error(callexpr.line, callexpr.column,
                    "Undeclared function: " + callexpr.callee);
        numOfErrors += 1;
    }

    size_t numberOfParams = scopeVec[0].getNumParams(callexpr.callee);

    if (numberOfParams != callexpr.args.size()) {
        int expected = scopeVec[0].getNumParams(callexpr.callee);
        int got = callexpr.args.size();

        Error error(callexpr.line, callexpr.column,
                    "Expected " + std::to_string(expected) +
                        " arguments, got: " + std::to_string(got));
        numOfErrors += 1;
    }

    for (size_t i = 0; i < callexpr.args.size(); i++) {
        Expression *expr = (callexpr.args[i]).get();
        expr->accept(*this);
    }

    std::vector<TypeKind *> paramTypes = scopeVec[0].getParams(callexpr.callee);

    for (size_t i = 0; i < numberOfParams; i++) {
        TypeKind *currentParamType = (callexpr.args[i])->infType;

        if (currentParamType != paramTypes[i]) {
            std::string msg = "Expected argument type: ";
            msg += paramTypes[i]->name + " got: ";
            msg += currentParamType->name;

            Error error(callexpr.args[i]->line, callexpr.args[i]->column, msg);

            numOfErrors += 1;
            return;
        }
    }
}

void SemanticVisitor::visitVarExpr(VarExpr &varexpr) {
    bool flag = false;

    for (int i = scopeVec.size() - 1; i >= 0; i--) {
        if (scopeVec[i].search(varexpr.Name)) {
            flag = true;

            if (scopeVec[i].getSymKind(varexpr.Name) == SymbolKind::VARIABLE) {
                varexpr.infType = scopeVec[i].getSymType(varexpr.Name);
                break;
            } else {
                Error error(varexpr.line, varexpr.column,
                            varexpr.Name + "is not a variable");
                numOfErrors += 1;
            }

            break;
        }
    }

    if (!flag) {
        Error error(varexpr.line, varexpr.column,
                    "Undeclared variable: " + varexpr.Name);
        numOfErrors += 1;
        varexpr.infType = getType("error");
    }
}

void SemanticVisitor::visitCharExpr(CharExpr &charexpr) {
    charexpr.infType = getType("char");
}

void SemanticVisitor::visitIntExpr(IntExpr &intexpr) {
    intexpr.infType = getType("int");
}
