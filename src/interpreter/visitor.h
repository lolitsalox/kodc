#pragma once

#include "../parser/ast.h"
#include "../misc/stack.h"

typedef struct ast_pair_t {
    ast_string_t key;
    ast_node_t* item;
} ast_pair_t;

typedef struct function_frame_t {
    ast_string_t name;
    linked_list_t vars; // for now a list of variables
} function_frame_t;

typedef struct visitor_t {
    stack_t stack; // stack of ast nodes
    stack_t function_frame_stack; // stack of function frames
    linked_list_t vars; // for now a list of variables
    ast_node_t* root; // root node
} visitor_t;

visitor_t* visitor_new(ast_node_t* root);
void visitor_run(visitor_t* visitor);