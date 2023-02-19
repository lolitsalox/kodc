#include "visitor.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

static void visitor_visit(visitor_t* visitor, ast_node_t* ast_node, bool should_push);

static ast_pair_t* ast_pair_new(ast_pair_t pair) {
    ast_pair_t* pair_pointer = malloc(sizeof(ast_pair_t));
    *pair_pointer = pair;
    return pair_pointer;
}

visitor_t* visitor_new(ast_node_t* root) {
    visitor_t* visitor = malloc(sizeof(visitor_t));
    stack_init(&visitor->stack);
    stack_init(&visitor->function_frame_stack);
    linked_list_init(&visitor->vars);
    visitor->root = root;
    return visitor;
}

static function_frame_t* function_frame_new(ast_function_t function) {
    function_frame_t* function_frame = malloc(sizeof(function_frame_t));
    function_frame->name = function.name;
    linked_list_init(&function_frame->vars);
    return function_frame;
}

static ast_pair_t* find_var(visitor_t* visitor, ast_string_t name) {
    linked_list_node_t* var = visitor->vars.head;
    size_t stack_index = 0;

    while (true) {
        while (var) {
            if (var->item && ast_string_compare(((ast_pair_t*)var->item)->key, name) == 0) {
                return var->item;
            }
            var = var->next;
        }

        if (stack_index < visitor->function_frame_stack.size)
            var = ((function_frame_t*)visitor->function_frame_stack.items[stack_index++])->vars.head;
        else break;
    }

    return NULL;
}

void visitor_run(visitor_t* visitor) {
    visitor_visit(visitor, visitor->root, false);
}

static void visitor_visit(visitor_t* visitor, ast_node_t* ast_node, bool should_push) {
    if (!visitor || !ast_node) {
        printf("[visitor]: Error - visitor or ast_node is null\n");
        exit(1);
    }

    switch (ast_node->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            linked_list_node_t* curr = ast_node->ast_compound.head;
            while (curr) {
                visitor_visit(visitor, curr->item, false);       
                curr = curr->next;
            }
            break;
        }

        case AST_LIST:
        case AST_STRING:
        case AST_NUMBER: {
            stack_push(&visitor->stack, ast_node);
            break;
        }

        case AST_IDENTIFIER: {
            ast_pair_t* var = find_var(visitor, ast_node->ast_string);

            if (!var) {
                printf("[visitor]: Error - variable `%.*s` not found\n", ast_node->ast_string.length, ast_node->ast_string.value);
                exit(1);
            }

            visitor_visit(visitor, var->item, true);
            break;
        }

        case AST_ASSIGNMENT: {
            visitor_visit(visitor, ast_node->ast_assignment.right, true);

            ast_node_t* right = stack_pop(&visitor->stack);

            if (!right) {
                printf("[visitor]: can't assign a null pointer\n");
                exit(1);
            }

            if (ast_node->ast_assignment.left->ast_type != AST_IDENTIFIER) {
                printf("[visitor]: TODO - implement other types of assignments\n");
                exit(1);
            }

            ast_pair_t* var = find_var(visitor, ast_node->ast_assignment.left->ast_string);
            
            if (!var) {
                function_frame_t* current_frame = stack_top(&visitor->function_frame_stack);
                linked_list_append(&current_frame->vars, ast_pair_new((ast_pair_t){
                    .key=ast_node->ast_assignment.left->ast_string,
                    .item=right,
                }));
                break;
            }

            var->item = right;
            break;
        }

        case AST_BIN_OP: {
            visitor_visit(visitor, ast_node->ast_assignment.right, true);
            visitor_visit(visitor, ast_node->ast_assignment.left, true);

            ast_node_t* left = stack_pop(&visitor->stack);
            ast_node_t* right = stack_pop(&visitor->stack);

            switch (ast_node->ast_bin_op.op) {
                case TOKEN_ADD: {
                    if (left->ast_type == AST_NUMBER && right->ast_type == AST_NUMBER) {
                        stack_push(&visitor->stack, ast_node_new((ast_node_t){
                            .ast_type=AST_NUMBER,
                            .ast_number={
                                .value=left->ast_number.value + right->ast_number.value
                            },
                        }));
                        return;
                    }

                    break;
                }

                default:
                    printf("[visitor]: TODO - implement %s for binary op\n", token_type_to_str(ast_node->ast_bin_op.op));
                    exit(1);
            }

            printf("[visitor]: Error - can't `%s` a `%s` and a `%s`\n",
                token_type_to_str(ast_node->ast_bin_op.op),
                ast_type_to_str(left->ast_type),
                ast_type_to_str(right->ast_type)
            );
            exit(1);
        }

        case AST_FUNCTION: {
            if (should_push) {
                stack_push(&visitor->stack, ast_node);
                break;
            }

            ast_pair_t* pair = ast_pair_new((ast_pair_t){
                .key=ast_node->ast_function.name,
                .item=ast_node
            });
            linked_list_append(&visitor->vars, pair);
            break;
        }

        case AST_CALL: {
            // * special case
            if (ast_node->ast_call.callable->ast_type == AST_IDENTIFIER && 
                ast_string_compare(
                    ast_node->ast_call.callable->ast_string, 
                    (ast_string_t){
                        .value="print",
                        .length=sizeof("print")
                    }) == 0
                ) {
                linked_list_node_t* arg = ast_node->ast_call.arguments->ast_compound.head;
                while (arg) {
                    visitor_visit(visitor, arg->item, true);
                    ast_print(stack_pop(&visitor->stack), 0);
                    arg = arg->next;
                }
                break;
            }

            visitor_visit(visitor, ast_node->ast_call.arguments, true); // pushes a list to the stack
            visitor_visit(visitor, ast_node->ast_call.callable, true);  // pushes a function node to the stack


            ast_node_t* function = stack_pop(&visitor->stack);
            ast_node_t* args = stack_pop(&visitor->stack);

            if (!function) {
                printf("[visitor]: Error - function is null");
                exit(1);
            }
            
            if (function->ast_type != AST_FUNCTION) {
                printf("[visitor]: Error - can't call an ast from type %s\n", 
                    ast_type_to_str(function->ast_type));
                exit(1);
            }

            if (function->ast_function.parameters->ast_compound.size != args->ast_compound.size) {
                printf("[visitor]: Error - parameters size != arguments size when calling function `%.*s`\n", 
                    function->ast_function.name.length, 
                    function->ast_function.name.value);
                exit(1);
            }

            stack_push(&visitor->function_frame_stack, function_frame_new(function->ast_function));
            visitor_visit(visitor, function->ast_function.body, should_push);
            break;
        }
        
        default:
            printf("[visitor]: TODO - visit for ast from type %s is not implemented yet\n",
                ast_type_to_str(ast_node->ast_type));
            exit(1);
            
    }

}