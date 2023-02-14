#pragma once

#include <stdint.h>
#include "../lexer/token.h"
#include "../misc/linked_list.h"

typedef enum ast_type_t {
    AST_ROOT,
    AST_COMPOUND,
    AST_LIST,
    AST_BLOCK,
    AST_CALL,
    AST_UNARY_OP,
    AST_BIN_OP,
    AST_FUNCTION,
    AST_ASSIGNMENT,
    AST_VARIABLE,
    AST_STATEMENT,
    AST_CONDITIONAL_STATEMENT,
    AST_UNARY_STATEMENT,
    AST_NUMBER,
    AST_STRING,
    AST_BOOL,
} ast_type_t;

typedef struct ast_node_t ast_node_t;

typedef struct ast_number_t {
    double value;
} ast_number_t;

typedef struct ast_string_t {
    char* value;
    uint32_t length;
} ast_string_t;

typedef struct ast_assignment_t {
    ast_node_t* left;
    ast_node_t* right;
} ast_assignment_t;

typedef struct ast_bin_op_t {
    ast_node_t* left;
    ast_node_t* right;
    token_type_t op;
} ast_bin_op_t;

typedef struct ast_function_t {
    char* name;
    ast_node_t* parameters;
    ast_node_t* body;
} ast_function_t;

struct ast_node_t {
    ast_type_t ast_type;
    union {
        linked_list_t ast_compound;
        ast_number_t ast_number;
        ast_string_t ast_string;
        ast_assignment_t ast_assignment;
        ast_function_t ast_function;
        ast_bin_op_t ast_bin_op;
    };
    
};

const char* ast_type_to_str(ast_type_t type);
void ast_print(const ast_node_t* ast_node, uint32_t indent_level);
ast_node_t* ast_node_new(ast_node_t ast_node);