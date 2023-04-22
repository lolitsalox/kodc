#include "ast.h"

const char* ast_type_to_str(AstType type) {
    switch (type) {
        case AST_ROOT:          return "ROOT";
        case AST_COMPOUND:      return "COMPOUND";
        case AST_LIST:          return "LIST";
        case AST_TUPLE:         return "TUPLE"; 
        case AST_BLOCK:         return "BLOCK";
        case AST_CALL:          return "CALL";
        case AST_METHOD_CALL:   return "METHOD_CALL";
        case AST_SUBSCRIPT:     return "SUBSCRIPT";
        case AST_ACCESS:        return "ACCESS";
        case AST_STORE_ATTR:    return "STORE_ATTR";
        case AST_FUNCTION:      return "FUNCTION";
        case AST_LAMBDA:        return "LAMBDA";
        case AST_ASSIGNMENT:    return "ASSIGNMENT";
        case AST_IDENTIFIER:    return "IDENTIFIER";
        case AST_INT:           return "INT";
        case AST_FLOAT:         return "FLOAT";
        case AST_STRING:        return "STRING";
        case AST_BOOL:          return "BOOL";
        case AST_NULL:          return "NULL";
        case AST_BIN_OP:        return "BIN_OP";
        case AST_UNARY_OP:      return "UNARY_OP";
        case AST_IF_STATEMENT:          return "IF_STATEMENT";
        case AST_WHILE_STATEMENT:       return "WHILE_STATEMENT";
        case AST_FOR_STATEMENT:         return "FOR_STATEMENT";
        case AST_RETURN_STATEMENT:      return "RETURN_STATEMENT";
    }
    return "UNKNOWN";
}

void ast_print(const AstNode* node, u32 indent_level) {
    // If the AST node is null, print a warning and return.
    if (!node) {
        printf("[ast]: Warning - node is null\n");
        return;
    }

    // Print indentation.
    for (u32 i = 0; i < indent_level; ++i) printf("    ");


    // Print the type of the AST node.
    printf("(%s): ", ast_type_to_str(node->type));

    // Switch on the type of the AST node.
    switch (node->type) {
        case AST_INT:
            printf("%ld\n", node->_int);
            break;

        case AST_FLOAT:
            printf("%f\n", node->_float);
            break;

        case AST_BOOL:
            puts(node->_bool ? "true" : "false");
            break;

        case AST_NULL:
            puts("null");
            break;

        case AST_IDENTIFIER:
        case AST_STRING:
            // If the node is a variable or string, print its value.
            if (node->_string)
                printf("%s\n", node->_string);
            break;

        case AST_ROOT:
        case AST_COMPOUND:
        case AST_LIST:
        case AST_TUPLE:
        case AST_BLOCK: {
            printf("\n");
            ast_list_print((AstList*)&node->_list, indent_level + 1);
            break;
        }

        case AST_ASSIGNMENT:
            // If the node is an assignment, print the left and right sides of the assignment
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("left:\n");
            ast_print(node->_assignment.left, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("right:\n");
            ast_print(node->_assignment.right, indent_level + 2);
            break;

        case AST_FUNCTION:
            // If the node is a function, print its name, parameters and the body.
            printf("name: %s\n", node->_function.name);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("params:\n");
            ast_print(node->_function.params, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("body:\n");
            ast_print(node->_function.body, indent_level + 2);
            break;

        case AST_LAMBDA:
            puts("");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("params:\n");
            ast_print(node->_lambda.params, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("body:\n");
            ast_print(node->_lambda.body, indent_level + 2);
            break;

        case AST_UNARY_OP:
            // If the node is a unary operation, print its operator, and value.
            printf("op: %s\n", token_type_to_str(node->_unary_op.op));
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(node->_unary_op.value, indent_level + 2);
            break;

        case AST_BIN_OP:
            // If the node is a binary operation, print its operator, left and right sides.
            printf("op: %s\n", token_type_to_str(node->_binary_op.op));
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("left:\n");
            ast_print(node->_binary_op.left, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("right:\n");
            ast_print(node->_binary_op.right, indent_level + 2);
            break;

        case AST_SUBSCRIPT:
            // If the node is a subscript operation, print the subscript and the value.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("subscript:\n");
            ast_print(node->_subscript.subscript, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(node->_subscript.value, indent_level + 2);
            break;

        case AST_ACCESS:
            // If the node is an access operation, print the field and the value.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("field:\n");
            ast_print(node->_access.field, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(node->_subscript.value, indent_level + 2);
            break;

        case AST_IF_STATEMENT:
        case AST_WHILE_STATEMENT:
            // If the node is an if/while statement, print the expression and the body.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("expression:\n");
            ast_print(node->_conditional.expr, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("body:\n");
            ast_print(node->_conditional.body, indent_level + 2);
            break;

        case AST_RETURN_STATEMENT:
            // If the node is return statement, print the value.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("value:\n");
            ast_print(node->_return, indent_level + 2);
            break;

        case AST_CALL:
            // If the node is a call operation, print the callable and the arguments.
            printf("\n");
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("callable:\n");
            ast_print(node->_call.callable, indent_level + 2);
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("arguments:\n");
            ast_print(node->_call.args, indent_level + 2);
            break;
        
        case AST_METHOD_CALL:
            // If the node is a call operation, print the callable and the arguments.
            printf("\n");
            
            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("this:\n");
            ast_print(node->_method_call.self, indent_level + 2);

            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("callable:\n");
            ast_print(node->_method_call.callable, indent_level + 2);

            for (uint32_t i = 0; i < indent_level + 1; ++i) printf("    ");
            printf("arguments:\n");
            ast_print(node->_method_call.args, indent_level + 2);
            break;
        default:
            printf("TODO: implement ast_print for %s\n", ast_type_to_str(node->type));
            break;
    }
}

enum STATUS ast_node_new(AstNode node, AstNode** out, char** err) {
    *out = malloc(sizeof(AstNode));
    if (!*out) {
        *err = "Coudln't allocate for ast_node";
        return STATUS_FAIL;
    }

    **out = node;
    return STATUS_OK;
}

void ast_free(AstNode* node) {
    if (!node) return;
    // printf("Freeing ast from type: %s (%p)\n", ast_type_to_str(node->type), node);
    switch (node->type) {
        case AST_INT:
        case AST_FLOAT:
        case AST_BOOL:
        case AST_NULL:
            break;

        case AST_IDENTIFIER:
        case AST_STRING:
            if (node->_string)
                free(node->_string);
            break;

        case AST_ROOT:
        case AST_COMPOUND:
        case AST_TUPLE:
        case AST_LIST:
        case AST_BLOCK: {
            ast_list_free(&node->_list);
            break;
        }

        case AST_STORE_ATTR:
        case AST_ASSIGNMENT:
            ast_free(node->_assignment.left);
            ast_free(node->_assignment.right);
            break;

        case AST_FUNCTION:
        case AST_LAMBDA:
            free(node->_function.name);
            ast_free(node->_function.params);
            ast_free(node->_function.body);
            break;

        case AST_UNARY_OP:
            ast_free(node->_unary_op.value);
            break;

        case AST_BIN_OP:
            ast_free(node->_binary_op.left);
            ast_free(node->_binary_op.right);
            break;

        case AST_SUBSCRIPT:
            ast_free(node->_subscript.subscript);
            ast_free(node->_subscript.value);
            break;

        case AST_ACCESS:
            ast_free(node->_access.field);
            ast_free(node->_subscript.value);
            break;

        case AST_IF_STATEMENT:
        case AST_WHILE_STATEMENT:
            ast_free(node->_conditional.expr);
            ast_free(node->_conditional.body);
            break;

        case AST_RETURN_STATEMENT:
            ast_free(node->_return);
            break;

        case AST_CALL:
            ast_free(node->_call.callable);
            ast_free(node->_call.args);
            break;
        
        case AST_METHOD_CALL:
            ast_free(node->_method_call.self);
            ast_free(node->_method_call.callable);
            ast_free(node->_method_call.args);
            break;

        case AST_FOR_STATEMENT:
            UNIMPLEMENTED;
            break;
    }

    free(node);
}

AstList ast_list_init() {
    return (AstList){0};
}

enum STATUS ast_list_append(AstList* list, AstNode* node, char** err) {
    AstListNode* new_list_node = malloc(sizeof(AstListNode));
    if (!new_list_node) {
        *err = "Couldn't allocate for AstListNode";
        return STATUS_FAIL;
    }

    *new_list_node = (AstListNode) {.node=node, .next=NULL, .prev=NULL};
    ++list->size;

    if (!list->head) {
        list->head = new_list_node;
        list->tail = new_list_node;
        return STATUS_OK;
    }

    new_list_node->prev = list->tail; // setting the prev as the old tail
    list->tail->next = new_list_node; // setting the next of the tail as the new node
    list->tail = new_list_node;       // replacing the old tail
    return STATUS_OK;
}

void ast_list_free(AstList* list) {
    if (!list) return;
    AstListNode* curr = list->head;

    while (curr) {
        ast_free(curr->node);
        curr = curr->next;
    }
}

void ast_list_print(AstList* list, u32 indent_level) {
    if (!list) return;
    AstListNode* curr = list->head;

    while (curr) {
        ast_print(curr->node, indent_level);
        curr = curr->next;
    }
}