#pragma once

#include "../defines.h"
#include "token.h"

typedef enum AstType {
    AST_ROOT, // root
    AST_COMPOUND, // body
    AST_TUPLE, // (..., )
    AST_LIST, // [..., ]
    AST_BLOCK, // {}
    AST_CALL,  // f()
    AST_METHOD_CALL, // f.method()
    AST_SUBSCRIPT,  // a[x]
    AST_ACCESS, // f.xyz
    AST_UNARY_OP, // -x
    AST_BIN_OP,  // a + b
    AST_FUNCTION,  // f() {}
    AST_LAMBDA, // |tuple| { body }
    AST_ASSIGNMENT,  // a = x
    AST_STORE_ATTR,  // a.x = x
    AST_STORE_SUBSCRIPT,  // a[x] = x
    AST_IDENTIFIER,  // a
    AST_INT,  // 5
    AST_FLOAT, // 5.2
    AST_STRING,  // "hello"
    AST_BOOL,  // true|false
    AST_NULL,  // null
    AST_IF_STATEMENT,  // if condition {}
    AST_WHILE_STATEMENT, // while condition {}
    AST_FOR_STATEMENT, // for i in range {}
    AST_RETURN_STATEMENT, // return x
} AstType;

typedef struct AstNode AstNode;
DA_STRUCT(Ast, AstNode*);

typedef AstArray AstList;
typedef AstList AstTuple;
typedef AstList AstBlock;
typedef AstList AstCompound;
typedef char* AstIdentifer;

typedef AstNode* AstReturnStatement;

typedef i64 AstInt;
typedef f64 AstFloat;
typedef bool AstBool;
typedef char* AstString;

typedef struct AstBinary {
    AstNode* left;
    AstNode* right;
} AstBinary;

typedef AstBinary AstAssignment;
typedef AstBinary AstStoreAttr;
typedef AstBinary AstStoreSubscript;

typedef struct AstUnaryOp {
    AstNode* value;
    TokenType_t op;
} AstUnaryOp;

typedef struct AstBinaryOp {
    AstNode* left;
    AstNode* right;
    TokenType_t op;
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

typedef AstConditionalStatement AstIfStatement;
typedef AstConditionalStatement AstWhileStatement;

// for x in range {}
typedef struct AstForStatement {

    AstNode* body; // must be block
} AstForStatement;

struct AstNode {
    AstType type;
    AstNode* parent;

    union {
        AstCompound compound;
        AstTuple tuple;
        AstList list;
        AstBlock block;
        AstCall call;
        AstMethodCall method_call;
        AstSubscript subscript;
        AstAccess access;
        AstUnaryOp unary_op;
        AstBinaryOp binary_op;
        AstFunction function;
        AstLambda lambda;
        AstAssignment assignment;
        AstStoreAttr store_attr;
        AstStoreSubscript store_subscript;
        AstIdentifer identifer;
        AstInt int_;
        AstFloat float_;
        AstString string;
        AstBool bool_;
        AstIfStatement if_s;
        AstWhileStatement while_s;
        AstForStatement for_s;
        AstReturnStatement return_s;
    };
    
};

const char* ast_type_to_str(AstType type);

Result ast_new(AstNode node, AstNode** out);
void ast_print(const AstNode* node, u32 indent_level);
void ast_free(AstNode* node);

void ast_list_print(const AstList* list, u32 indent_level);
void ast_list_free(AstList* list);