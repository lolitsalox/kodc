#include "interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "env.h"

#define MAKE_NATIVE_FN(fn_name, fn) \
    env_set_variable( \
        env, \
        (ast_string_t){ \
            .value=fn_name, \
            .length=sizeof(fn_name) \
        }, \
        object_new((kod_object_t){ \
            .object_type=OBJECT_NATIVE_FUNCTION, \
            .native_function=(kod_native_function_t){ \
                .name={.value=fn_name, .length=sizeof(fn_name)}, \
                .caller=fn \
            } \
        })\
    ); \

#define MAKE_NUMBER(val) object_new((kod_object_t){.object_type=OBJECT_NUMBER,.number=val})
#define MAKE_STRING(ast_str) object_new((kod_object_t){.object_type=OBJECT_STRING,.string=ast_str})

static kod_object_t* visit(env_t* env, ast_node_t* node) {
    switch (node->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            linked_list_node_t* curr = node->ast_compound.head;
            while (curr) {
                kod_object_t* value = visit(env, curr->item);
                if (env->does_return) {
                    env->does_return = false;
                    return value;
                }
                curr = curr->next;
            }
            break;
        }

        case AST_ASSIGNMENT: {
            kod_object_t* value = visit(env, node->ast_assignment.right);

            if (node->ast_assignment.left->ast_type != AST_IDENTIFIER) {
                printf("[interpreter]: Error - left side is not an identifier\n");
                exit(1);
            }
            ast_string_t ident = node->ast_assignment.left->ast_string;

            env_set_variable(env, ident, value);
            return value;
        }

        case AST_IDENTIFIER: {
            kod_object_t* value = env_get_variable(env, node->ast_string);
            if (!value) {
                printf("[interpreter]: Error - identifer `%.*s` does not exist.\n",
                    node->ast_string.length,
                    node->ast_string.value);
                exit(1);
            }
            return value;
        }

        case AST_STRING: {
            return MAKE_STRING(node->ast_string);
        }

        case AST_NUMBER: {
            return MAKE_NUMBER(node->ast_number.value);
        }

        case AST_RETURN_STATEMENT: {
            env->does_return = true;
            return visit(env, node->ast_return_statement.value);
        }

        case AST_IF_STATEMENT: {
            kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
            if (expr->object_type == OBJECT_NUMBER && expr->number) {
                visit(env, node->ast_conditional_statement.body);
            }
            break;
        }

        case AST_WHILE_STATEMENT: {
            kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
            while (expr->object_type == OBJECT_NUMBER && expr->number) {
                visit(env, node->ast_conditional_statement.body);
                
                expr = visit(env, node->ast_conditional_statement.expression);
            }
            break;
        }

        case AST_UNARY_OP: {
            kod_object_t* value = visit(env, node->ast_unary_op.value);
            // TODO: split into functions
            switch (node->ast_unary_op.op) {
                case TOKEN_ADD: {
                    if (value->object_type == OBJECT_NUMBER) {
                        return value;
                    }

                    break;
                }
                
                case TOKEN_SUB: {
                    if (value->object_type == OBJECT_NUMBER) {
                        return MAKE_NUMBER(-value->number);
                    }

                    break;
                }
                case TOKEN_NOT: {
                    if (value->object_type == OBJECT_NUMBER) {
                        return MAKE_NUMBER(~(int64_t)value->number);
                    }

                    break;
                }
                case TOKEN_BOOL_NOT: {
                    if (value->object_type == OBJECT_NUMBER) {
                        return MAKE_NUMBER(!value->number);
                    }

                    break;
                }

                default:
                    printf("[interpreter]: TODO - implement %s for binary op\n", token_type_to_str(node->ast_bin_op.op));
                    exit(1);
            }

            printf("[interpreter]: Error - can't use a unary `%s` on a `%s`\n",
                token_type_to_str(node->ast_bin_op.op),
                object_type_to_str(value->object_type)
            );
            exit(1);
            break;
        }

        case AST_FUNCTION: {
            kod_object_t* fn_object = object_new((kod_object_t){
                .object_type=OBJECT_FUNCTION,
                .function={
                    .function_node=node->ast_function
                }
            });

            env_set_variable(env, node->ast_function.name, fn_object);
            return fn_object;
        }

        case AST_CALL: {
            kod_object_t* fn_object = visit(env, node->ast_call.callable);
            switch (fn_object->object_type) {

                case OBJECT_FUNCTION: {
                    env_t* new_env = env_new(env);

                    linked_list_t args = node->ast_call.arguments->ast_compound;
                    linked_list_t params = fn_object->function.function_node.parameters->ast_compound;

                    if (args.size != params.size) {
                        printf("[interpreter]: Error - arguments size does not match parameters size\n");
                        exit(1);
                    }

                    linked_list_node_t* curr_arg = args.head;
                    linked_list_node_t* curr_param = params.head;
                    
                    while (curr_arg && curr_param) {
                        kod_object_t* arg = visit(env, curr_arg->item);
                        ast_node_t* param = (ast_node_t*)curr_param->item;
                        if (param->ast_type != AST_IDENTIFIER) {
                            printf("[interpreter]: Error - param is not an identifer\n");
                            exit(1);
                        }

                        // mapping arg to param
                        env_set_variable(new_env, param->ast_string, arg);

                        curr_arg = curr_arg->next;
                        curr_param = curr_param->next;
                    }

                    return visit(new_env, fn_object->function.function_node.body);
                }

                case OBJECT_NATIVE_FUNCTION: {
                    // printf("%.*s -> %p\n", fn_object->native_function.name.length, fn_object->native_function.name.value, fn_object->native_function.caller);
                    return fn_object->native_function.caller(env, node->ast_call.arguments->ast_compound);
                }

                default: printf("[interpreter]: Error - object from type %s is not callable\n",
                object_type_to_str(fn_object->object_type));
                exit(1);
            }
            break;
        }
        
        default:
            printf("[interpreter]: TODO - visit for ast from type %s is not implemented yet\n",
                ast_type_to_str(node->ast_type));
            exit(1);
    }
    
    return NULL;
}

kod_object_t* kod_print(env_t* env, linked_list_t params) {
    linked_list_node_t* curr = params.head;
    while (curr) {
        object_print(visit(env, curr->item), 0);
        curr = curr->next;
    }
    return NULL;
}

kod_object_t* kod_time(env_t* env, linked_list_t params) {
    return MAKE_NUMBER(time(NULL));
}

kod_object_t* eval(ast_node_t* root) {
    env_t* env = env_new(NULL);

    MAKE_NATIVE_FN("print", kod_print);
    MAKE_NATIVE_FN("time", kod_time);
    return visit(env, root);
}