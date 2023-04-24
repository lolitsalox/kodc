#pragma once

#include <defines.h>
#include <lexer/token.h>

typedef enum AstType {
    AST_ROOT,
    AST_COMPOUND,
    AST_TUPLE,
    AST_LIST,
    AST_LAMBDA,
    AST_BLOCK,
    AST_CALL,
    AST_METHOD_CALL,
    AST_SUBSCRIPT,
    AST_ACCESS,
    AST_UNARY_OP,
    AST_BIN_OP,
    AST_FUNCTION,
    AST_ASSIGNMENT,
    AST_STORE_ATTR,
    AST_IDENTIFIER,
    AST_INT,
    AST_FLOAT,
    AST_STRING,
    AST_BOOL,
    AST_NULL,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_FOR_STATEMENT,
    AST_RETURN_STATEMENT,
} AstType;

typedef struct AstNode AstNode;

typedef struct AstListNode {
    AstNode* node;
    struct AstListNode* next;
    struct AstListNode* prev;
} AstListNode;

typedef struct AstList {
    AstListNode* head;
    AstListNode* tail;
    size_t size;
} AstList;

typedef struct AstBinary {
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef struct AstUnaryOp {
    AstNode* value;
    TokenType op;
} AstUnaryOp;

typedef struct AstBinaryOp {
    AstNode* left;
    AstNode* right;
    TokenType op;
} AstBinaryOp;

typedef struct AstFunction {
    char* name;
    AstNode* params; // must be tuple 
    AstNode* body; // must be block
} AstFunction;

typedef struct AstLambda {
    AstNode* params; // must be tuple 
    AstNode* body; // must be block
} AstLambda;

typedef struct AstCall {
    AstNode* callable;
    AstNode* args; // must be tuple
} AstCall;

typedef struct AstMethodCall {
    AstNode* callable; // must be id
    AstNode* args; // must be tuple
    AstNode* self;
} AstMethodCall;

typedef struct AstSubscript {
    AstNode* value;
    AstNode* subscript; // must be tuple
} AstSubscript;

typedef struct AstAccess {
    AstNode* value;
    AstNode* field; // must be id
} AstAccess;

typedef struct AstConditionalStatement {
    AstNode* expr;
    AstNode* body; // must be block
} AstConditionalStatement;


struct AstNode {
    AstType type;

    union {
        AstList _list;
        bool    _bool;
        i64     _int;
        f64     _float;
        char*   _string;
        AstNode* _return;
        AstBinary   _assignment;
        AstBinary   _store_attr; // left must be access
        AstFunction _function;
        AstLambda _lambda;
        AstCall     _call;
        AstAccess   _access;
        AstUnaryOp  _unary_op;
        AstBinaryOp _binary_op;
        AstSubscript    _subscript;
        AstMethodCall   _method_call;
        AstConditionalStatement _conditional;
    };
    
};

const char* ast_type_to_str(AstType type);
void ast_print(const AstNode* node, u32 indent_level);
enum STATUS ast_node_new(AstNode node, AstNode** out, char** err);
void ast_free(AstNode* node);

AstList ast_list_init();
void ast_list_print(AstList* list, u32 indent_level);
enum STATUS ast_list_append(AstList* list, AstNode* node, char** err);
void ast_list_free(AstList list);