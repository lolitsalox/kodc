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
        case AST_SUBSCRIPT:     return "SUBSCRIPT";
        case AST_ACCESS:        return "ACCESS";
        case AST_FUNCTION:      return "FUNCTION";
        case AST_LAMBDA:        return "LAMBDA";
        case AST_ASSIGNMENT:    return "ASSIGNMENT";
        case AST_IDENTIFIER:    return "IDENTIFIER";
        case AST_STATEMENT:     return "STATEMENT";
        case AST_NUMBER:        return "NUMBER";
        case AST_STRING:        return "STRING";
        case AST_BOOL:          return "BOOL";
        case AST_BIN_OP:        return "BIN_OP";
        case AST_UNARY_OP:      return "UNARY_OP";
        case AST_UNARY_STATEMENT:           return "UNARY_STATEMENT";
        case AST_CONDITIONAL_STATEMENT:     return "CONDITIONAL_STATEMENT";
    }
    return "UNKNOWN";
}

void ast_print(const ast_node_t* ast_node, uint32_t indent_level) {
    // If the AST node is null, print a warning and return.
    if (!ast_node) {
        printf("[ast]: Warning - ast_node is null\n");
        return;
    }
    // Print indentation.
    for (uint32_t i = 0; i < indent_level; ++i) printf("    ");
    // Print the type of the AST node.
    printf("(%s): ", ast_type_to_str(ast_node->ast_type));

    // Switch on the type of the AST node.
    switch (ast_node->ast_type) {
        case AST_NUMBER:
            // If the node is a number, print its value.
            printf("%g\n", ast_node->ast_number.value);
            break;

        case AST_IDENTIFIER:
        case AST_STRING:
            // If the node is a variable or string, print its value.
            if (ast_node->ast_string.value)
                printf("%.*s\n", ast_node->ast_string.length, ast_node->ast_string.value);
            break;

        case AST_ROOT:
        case AST_COMPOUND:
        case AST_LIST:
        case AST_BLOCK: {
            // If the node is a root, compound, list, or block,
            // print a new line and iterate over each item in the linked list,
            // recursively printing each item with an increased indentation level.
            printf("\n");
            linked_list_node_t* curr = ast_node->ast_compound.head;
            while (curr) {
                ast_print(curr->item, indent_level + 1);
                curr = curr->next;
            }
            break;
        }

        case AST_ASSIGNMENT:
            // If the node is an assignment, print the left and right sides of the assignment
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("left:\n");
            ast_print(ast_node->ast_assignment.left, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("right:\n");
            ast_print(ast_node->ast_assignment.right, indent_level + 2);
            break;

        case AST_FUNCTION:
        case AST_LAMBDA:
            // If the node is a function, print its name, parameters and the body.
            printf("name: %.*s\n", ast_node->ast_function.name.length, ast_node->ast_function.name.value);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("params:\n");
            ast_print(ast_node->ast_function.parameters, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("body:\n");
            ast_print(ast_node->ast_function.body, indent_level + 2);
            break;

        case AST_UNARY_OP:
            // If the node is a unary operation, print its operator, and value.
            printf("op: %s\n", token_type_to_str(ast_node->ast_unary_op.op));
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(ast_node->ast_bin_op.left, indent_level + 2);
            break;

        case AST_BIN_OP:
            // If the node is a binary operation, print its operator, left and right sides.
            printf("op: %s\n", token_type_to_str(ast_node->ast_bin_op.op));
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("left:\n");
            ast_print(ast_node->ast_bin_op.left, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("right:\n");
            ast_print(ast_node->ast_bin_op.right, indent_level + 2);
            break;

        case AST_SUBSCRIPT:
            // If the node is a subscript operation, print the subscript and the value.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("subscript:\n");
            ast_print(ast_node->ast_subscript.subscript, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(ast_node->ast_subscript.value, indent_level + 2);
            break;

        case AST_ACCESS:
            // If the node is an access operation, print the field and the value.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("field:\n");
            ast_print(ast_node->ast_access.field, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(ast_node->ast_subscript.value, indent_level + 2);
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