#include "runtime.h"

#include "object.h"
#include "env.h"
#include "../parser/ast.h"
#include "kod_builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define isalpha(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
#define isdigit(c) ('0' <= c && c <= '9')

extern kod_object_t* visit(env_t* env, ast_node_t* node);

kod_object_t* interpret_ast(ast_node_t* root) {
    env_t* global_env = env_new(NULL);

    builtins_init(global_env);

    time_t start = time(NULL);

    kod_object_t* result = visit(global_env, root);

    time_t delta = time(NULL) - start;
    struct tm* ptm = localtime(&delta);
    printf("Finished in: %02d:%02d\n",ptm->tm_min, ptm->tm_sec);
    
    // env_free(global_env);
    return result;
}

kod_object_t* visit(env_t* env, ast_node_t* node) {
    if (!node) return NULL;

    switch (node->ast_type) {
        case AST_ROOT:
        case AST_BLOCK: {
            linked_list_node_t* curr = node->ast_compound.head;
            while (curr) {
                kod_object_t* value = visit(env, curr->item);
                if (env->does_return) {
                    return value;
                }
                if (value && value->from_return)
                    object_dec_ref(value);
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

            char* tmp = malloc(ident.length + 1);
            strncpy(tmp, ident.value, ident.length);
            tmp[ident.length] = 0;

            env_set_variable(env, tmp, value);
            return value;
        }

        case AST_IDENTIFIER: {
            char* tmp = malloc(node->ast_string.length + 1);
            strncpy(tmp, node->ast_string.value, node->ast_string.length);
            tmp[node->ast_string.length] = 0;

            kod_object_t* value = env_get_variable(env, tmp);
            free(tmp);
            
            if (!value) {
                printf("[interpreter]: Error - identifer `%.*s` does not exist.\n",
                    node->ast_string.length,
                    node->ast_string.value);
                exit(1);
            }
            return value;
        }

        case AST_STRING: {
            char* tmp = malloc(node->ast_string.length + 1);
            strncpy(tmp, node->ast_string.value, node->ast_string.length);
            tmp[node->ast_string.length] = 0;

            return make_string(tmp);
        }

        case AST_NUMBER: {
            return make_int(node->ast_number.value); // todo change ast nodes to int and float
        }

        case AST_RETURN_STATEMENT: {
            env->does_return = true;
            return visit(env, node->ast_return_statement.value);
        }

        case AST_IF_STATEMENT: {
            kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
            if (expr->object_type == OBJECT_INT && expr->_int) {
                expr = visit(env, node->ast_conditional_statement.body);
            }
            return expr;
        }

        case AST_WHILE_STATEMENT: {
            kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
            while (expr->object_type == OBJECT_INT && expr->_int) {
                visit(env, node->ast_conditional_statement.body);
                
                expr = visit(env, node->ast_conditional_statement.expression);
            }
            break;
        }

        // case AST_UNARY_OP:
            // return eval_unary_op(env, node->ast_unary_op);

        // case AST_BIN_OP:
        //     return eval_binary_op(env, node->ast_bin_op);

        case AST_FUNCTION: {
            kod_object_t* fn_object = object_new((kod_object_t){
                .object_type=OBJECT_FUNCTION,
                ._function={
                    .function_node=node->ast_function,
                    .env=env
                }
            });

            char* tmp = malloc(node->ast_function.name.length + 1);
            strncpy(tmp, node->ast_function.name.value, node->ast_function.name.length);
            tmp[node->ast_function.name.length] = 0;

            env_set_variable(env, tmp, fn_object);
            return fn_object;
        }

        case AST_ACCESS: {
            kod_object_t* value = visit(env, node->ast_access.value);
            if (value) {
                kod_object_t* res = visit(value->attributes, node->ast_access.field);
                object_dec_ref(value);
                return res;
            }
            object_dec_ref(value);
            break;
        }

        case AST_METHOD_CALL: {
            kod_object_t* this = visit(env, node->ast_method_call.this);
            if (this) {
                kod_object_t* fn_object = visit(this->attributes, node->ast_method_call.callable);
                if (!fn_object) {
                    printf("[interpreter]: Error - cannot call a null object.\n");
                    exit(1);
                }

                switch (fn_object->object_type) {
                    case OBJECT_FUNCTION: {
                        env_t* new_env = env_new(fn_object->_function.env);

                        linked_list_t args = node->ast_call.arguments->ast_compound;
                        linked_list_t params = fn_object->_function.function_node.parameters->ast_compound;

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
                            char* tmp = malloc(param->ast_string.length + 1);
                            strncpy(tmp, param->ast_string.value, param->ast_string.length);
                            tmp[param->ast_string.length] = 0;

                            env_set_variable(new_env, tmp, arg);

                            curr_arg = curr_arg->next;
                            curr_param = curr_param->next;
                        }

                        kod_object_t* value = visit(new_env, fn_object->_function.function_node.body);
                        object_inc_ref(value);
                        value->from_return = true;
                        env_free(new_env);
                        return value;
                    }

                    case OBJECT_NATIVE_FUNCTION: {
                        return fn_object->_native_function.caller(env, node->ast_call.arguments->ast_compound);
                    }

                    default: printf("[interpreter]: Error - object from type %s is not callable\n",
                    object_type_to_str(fn_object->object_type));
                    exit(1);
                }

            }
            object_dec_ref(this);
            return NULL;
        }

        case AST_CALL: {
            kod_object_t* fn_object = visit(env, node->ast_call.callable);
            if (!fn_object) {
                printf("[interpreter]: Error - cannot call a null object.\n");
                exit(1);
            }

            switch (fn_object->object_type) {

                case OBJECT_FUNCTION: {
                    env_t* new_env = env_new(fn_object->_function.env);

                    linked_list_t args = node->ast_call.arguments->ast_compound;
                    linked_list_t params = fn_object->_function.function_node.parameters->ast_compound;

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
                        char* tmp = malloc(param->ast_string.length + 1);
                        strncpy(tmp, param->ast_string.value, param->ast_string.length);
                        tmp[param->ast_string.length] = 0;

                        env_set_variable(new_env, tmp, arg);

                        curr_arg = curr_arg->next;
                        curr_param = curr_param->next;
                    }

                    kod_object_t* value = visit(new_env, fn_object->_function.function_node.body);
                    object_inc_ref(value);
                    value->from_return = true;
                    env_free(new_env);
                    return value;
                }

                case OBJECT_NATIVE_FUNCTION: {
                    // printf("%.*s -> %p\n", fn_object->native_function.name.length, fn_object->native_function.name.value, fn_object->native_function.caller);
                    return fn_object->_native_function.caller(env, node->ast_call.arguments->ast_compound);
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