#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

const char* ast_type_to_str(ast_type_t type) {
    switch (type) {
        case AST_ROOT:          return "ROOT";
        case AST_COMPOUND:      return "COMPOUND";
        case AST_LIST:          return "LIST";
        case AST_BLOCK:         return "BLOCK";
        case AST_CALL:          return "CALL";
        case AST_FUNCTION:      return "FUNCTION";
        case AST_ASSIGNMENT:    return "ASSIGNMENT";
        case AST_VARIABLE:      return "VARIABLE";
        case AST_STATEMENT:     return "STATEMENT";
        case AST_NUMBER:        return "NUMBER";
        case AST_STRING:        return "STRING";
        case AST_BIN_OP:        return "BIN_OP";
        case AST_UNARY_OP:      return "UNARY_OP";
        case AST_UNARY_STATEMENT:           return "UNARY_STATEMENT";
        case AST_CONDITIONAL_STATEMENT:     return "CONDITIONAL_STATEMENT";
        default:                return "UNKNOWN";
    }
}

void ast_print(const ast_node_t* ast_node) {
    if (!ast_node) {
        printf("[ast]: Warning - ast_node is null\n");
        return;
    }

    printf("(%s): ", ast_type_to_str(ast_node->ast_type));

    switch (ast_node->ast_type) {
        case AST_NUMBER:
            printf("%g\n", ast_node->ast_number.value);
            break;
        case AST_VARIABLE:
        case AST_STRING:
            if (ast_node->ast_string.value)
                printf("%.*s\n", ast_node->ast_string.length, ast_node->ast_string.value);
            break;
        case AST_ROOT:
        case AST_COMPOUND:
        case AST_LIST:
        case AST_BLOCK:
            printf("\n");
            linked_list_print(&((ast_node_t*)ast_node)->ast_compound, (void(*)(void*))ast_print);
            break;
        case AST_ASSIGNMENT:
            printf("\nleft:\n");
            ast_print(ast_node->ast_assignment.left);
            printf("right:\n");
            ast_print(ast_node->ast_assignment.right);
            break;
        case AST_FUNCTION:
            printf("name: %s\n", ast_node->ast_function.name);
            printf("params:\n");
            ast_print(ast_node->ast_function.parameters);
            printf("body:\n");
            ast_print(ast_node->ast_function.body);
            break;
        case AST_BIN_OP:
            printf("op: %s\n", token_type_to_str(ast_node->ast_bin_op.op));
            printf("left:\n");
            ast_print(ast_node->ast_bin_op.left);
            printf("right:\n");
            ast_print(ast_node->ast_bin_op.right);
            break;
        default:
            printf("TODO: implement ast_print\n");
            break;
    }
}

ast_node_t* ast_node_new(ast_node_t ast_node) {
    ast_node_t* ast_node_pointer = malloc(sizeof(ast_node_t));
    if (!ast_node_pointer) {
        printf("[ast_node]: Error - coudln't allocate for ast_node\n");
        return NULL;
    }

    *ast_node_pointer = ast_node;
    return ast_node_pointer;
}