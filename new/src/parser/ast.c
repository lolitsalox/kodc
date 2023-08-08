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
        case AST_STORE_SUBSCRIPT:return "STORE_SUBSCRIPT";
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
    return "UNKNOWN_AST_TYPE";
}

#define PRINT_SPACE(n) printf("%*c", n, ' ')
#define PRINT_INDENT() PRINT_SPACE(indent_level * 4)
#define PRINT_INDENT_DOUBLE() PRINT_SPACE(indent_level * 8)

void ast_print(const AstNode* node, u32 indent_level) {
    assert(node && "Ast node is NULL");

    // Print indentation.
    PRINT_INDENT();

    // Print the type of the AST node.
    printf("(%s): ", ast_type_to_str(node->type));

    // Switch on the type of the AST node.
    switch (node->type) {
        case AST_INT: printf("%lld\n", node->int_); break;
        
        case AST_FLOAT: printf("%f\n", node->float_); break;
        
        case AST_BOOL: puts(node->bool_ ? "true" : "false"); break;
        
        case AST_NULL: puts("null"); break;

        case AST_IDENTIFIER:
        case AST_STRING: if (node->string) puts(node->string); break;
        
        case AST_ROOT:
        case AST_COMPOUND:
        case AST_LIST:
        case AST_TUPLE:
        case AST_BLOCK:
            puts("");
            ast_list_print(&node->list, indent_level + 1);
            if (node->type == AST_ROOT) puts("");
            break;

        case AST_STORE_ATTR:
        case AST_STORE_SUBSCRIPT:
        case AST_ASSIGNMENT:
            puts("");

            PRINT_INDENT_DOUBLE();
            puts("left:");
            ast_print(node->assignment.left, indent_level + 2);

            PRINT_INDENT_DOUBLE();
            puts("right:");
            ast_print(node->assignment.right, indent_level + 2);
            break;

        case AST_FUNCTION:
            printf("name: %s", node->function.name);

            PRINT_INDENT_DOUBLE();
            puts("params:");
            ast_print(node->function.params, indent_level + 2);

            PRINT_INDENT_DOUBLE();
            puts("body:");
            ast_print(node->function.body, indent_level + 2);
            break;

        case AST_LAMBDA:
            puts("");

            PRINT_INDENT_DOUBLE();
            puts("params:");
            ast_print(node->lambda.params, indent_level + 2);

            PRINT_INDENT_DOUBLE();
            puts("body:");
            ast_print(node->lambda.body, indent_level + 2);
            break;

        case AST_UNARY_OP:
            printf("op: %s\n", token_type_to_str(node->unary_op.op));

            PRINT_INDENT_DOUBLE();
            puts("value:");
            ast_print(node->unary_op.value, indent_level + 2);
            break;

        case AST_BIN_OP:
            printf("op: %s\n", token_type_to_str(node->binary_op.op));
            
            PRINT_INDENT_DOUBLE();
            puts("left:");
            ast_print(node->binary_op.left, indent_level + 2);
            
            PRINT_INDENT_DOUBLE();
            puts("right:");
            ast_print(node->binary_op.right, indent_level + 2);
            break;

        case AST_SUBSCRIPT:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            puts("subscript:");
            ast_print(node->subscript.subscript, indent_level + 2);
            
            PRINT_INDENT_DOUBLE();
            puts("value:");
            ast_print(node->subscript.value, indent_level + 2);
            break;

        case AST_ACCESS:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            printf("field:\n");
            ast_print(node->access.field, indent_level + 2);
            
            PRINT_INDENT_DOUBLE();
            printf("value:\n");
            ast_print(node->access.value, indent_level + 2);
            break;

        case AST_IF_STATEMENT:
        case AST_WHILE_STATEMENT:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            puts("expression:");
            ast_print(node->if_s.expr, indent_level + 2);
            
            PRINT_INDENT_DOUBLE();
            puts("body:");
            ast_print(node->if_s.body, indent_level + 2);
            break;

        case AST_RETURN_STATEMENT:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            puts("value:");
            ast_print(node->return_s, indent_level + 2);
            break;

        case AST_CALL:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            puts("callable:");
            ast_print(node->call.callable, indent_level + 2);
            
            PRINT_INDENT();
            puts("arguments:");
            ast_print(node->call.args, indent_level + 2);
            break;
        
        case AST_METHOD_CALL:
            puts("");
            
            PRINT_INDENT_DOUBLE();
            puts("this:");
            ast_print(node->method_call.self, indent_level + 2);

            
            PRINT_INDENT_DOUBLE();
            puts("callable:");
            ast_print(node->method_call.callable, indent_level + 2);

            
            PRINT_INDENT_DOUBLE();
            puts("arguments:");
            ast_print(node->method_call.args, indent_level + 2);
            break;

        default:
            printf("TODO: implement ast_print for %s\n", ast_type_to_str(node->type));
            break;
    }
}

Result ast_new(AstNode node, AstNode** out) {
    assert(out && "Out is NULL");
    
    if (!out) return result_error("out parameter is NULL");
    
    #ifdef DEBUG_PARSER
    INFO_ARGS("Creating an ast node from type %s", ast_type_to_str(node.type));
    #endif

    *out = kod_malloc(sizeof(AstNode));
    if (!*out) {
        result_error("Coudln't allocate for ast node");
    }

    **out = node;
    return result_ok();
}

void ast_free(AstNode* node) {
    assert(node && "Ast node is NULL");

    #ifdef DEBUG_PARSER
    INFO_ARGS("Freeing ast from type: %s (%p)", ast_type_to_str(node->type), node);
    #endif

    switch (node->type) {
        case AST_INT:
        case AST_FLOAT:
        case AST_BOOL:
        case AST_NULL:
            break;

        case AST_IDENTIFIER:
        case AST_STRING:
            if (node->string)
                kod_free(node->string);
            break;

        case AST_ROOT:
        case AST_COMPOUND:
        case AST_TUPLE:
        case AST_LIST:
        case AST_BLOCK: {
            ast_list_free(&node->list);
            break;
        }

        case AST_STORE_SUBSCRIPT:
        case AST_STORE_ATTR:
        case AST_ASSIGNMENT:
            ast_free(node->assignment.left);
            ast_free(node->assignment.right);
            break;

        case AST_FUNCTION:
            kod_free(node->function.name);
            ast_free(node->function.params);
            ast_free(node->function.body);
            break;

        case AST_LAMBDA:
            ast_free(node->lambda.params);
            ast_free(node->lambda.body);
            break;

        case AST_UNARY_OP:
            ast_free(node->unary_op.value);
            break;

        case AST_BIN_OP:
            ast_free(node->binary_op.left);
            ast_free(node->binary_op.right);
            break;

        case AST_SUBSCRIPT:
            ast_free(node->subscript.subscript);
            ast_free(node->subscript.value);
            break;

        case AST_ACCESS:
            ast_free(node->access.field);
            ast_free(node->access.value);
            break;

        case AST_IF_STATEMENT:
        case AST_WHILE_STATEMENT:
            ast_free(node->if_s.expr);
            ast_free(node->if_s.body);
            break;

        case AST_RETURN_STATEMENT:
            ast_free(node->return_s);
            break;

        case AST_CALL:
            ast_free(node->call.callable);
            ast_free(node->call.args);
            break;
        
        case AST_METHOD_CALL:
            ast_free(node->method_call.self);
            ast_free(node->method_call.callable);
            ast_free(node->method_call.args);
            break;

        case AST_FOR_STATEMENT:
            UNIMPLEMENTED;
            break;
    }

    kod_free(node);
}

inline void ast_list_print(const AstList* list, u32 indent_level) {
    for (size_t i = 0; i < list->count; ++i)
        ast_print(list->items[i], indent_level);
}

inline void ast_list_free(AstList* list) {
    for (size_t i = 0; i < list->count; ++i)
        ast_free(list->items[i]);
    DA_FREE(*list);
}