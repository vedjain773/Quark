#ifndef NODE_H
#define NODE_H

enum class NodeType {
    //Expressions
    INT_EXPR, CHAR_EXPR, VAR_EXPR,
    UNARY_EXPR, BINARY_EXPR, ASSIGN_EXPR,
    CALL_EXPR, EMPTY_EXPR,

    //Statements
    EXPR_STMT, IF_STMT, ELSE_STMT,
    WHILE_STMT, BLOCK_STMT, RETURN_STMT,
    EMPTY_STMT, DECL_STMT,

    //Functions
    PARAM, PROTO, FUNC_DEF,
};

class Node {
    public:
    virtual ~Node() = default;
    virtual NodeType getNodeType() = 0;
};

#endif
