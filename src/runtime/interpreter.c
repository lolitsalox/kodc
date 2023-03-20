#include "interpreter.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include "env.h"
// #define isalpha(c) (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))

// env_t string_attributes;
// env_t int_attributes;

// static kod_object_t* visit(env_t* env, ast_node_t* node);

// #define MAKE_NUMBER(val) object_new((kod_object_t){.object_type=OBJECT_NUMBER,.number=val,.attributes=&int_attributes})
// #define MAKE_STRING(ast_str) object_new((kod_object_t){.object_type=OBJECT_STRING,.string=ast_str,.attributes=&string_attributes})

// static kod_object_t* eval_unary_op_number(double value, token_type_t op) {
//     switch (op) {
//         case TOKEN_ADD: return MAKE_NUMBER(value);
//         case TOKEN_SUB: return MAKE_NUMBER(-value);
//         case TOKEN_NOT: return MAKE_NUMBER(~(int64_t)value);
//         case TOKEN_BOOL_NOT: return MAKE_NUMBER(!value);
//         default:
//             printf("[interpreter]: Error - Unary `%s` is unimplemented.\n",
//                 token_type_to_str(op)
//             );
//             exit(1);
//     }
//     return NULL;
// }

// #include <math.h>
// static kod_object_t* eval_binary_op_number(double left, double right, token_type_t op) {
//     switch (op) {
//         case TOKEN_ADD: return MAKE_NUMBER(left + right);
//         case TOKEN_SUB: return MAKE_NUMBER(left - right);
//         case TOKEN_MUL: return MAKE_NUMBER(left * right);
//         case TOKEN_DIV: return MAKE_NUMBER(left / right);
//         case TOKEN_POW: return MAKE_NUMBER(pow(left, right));
//         case TOKEN_BOOL_LT: return MAKE_NUMBER(left < right);
//         default:
//             printf("[interpreter]: Error - Binary `%s` is unimplemented.\n",
//                 token_type_to_str(op)
//             );
//             exit(1);
//     }
//     return NULL;
// }

// static kod_object_t* eval_unary_op(env_t* env, ast_unary_op_t node) {
//     kod_object_t* value = visit(env, node.value);

//     if (!value) return NULL;

//     if (value->object_type == OBJECT_NUMBER) {
//         return eval_unary_op_number(value->number, node.op);
//     }

//     printf("[interpreter]: Error - can't use a unary `%s` on a `%s`\n",
//                 token_type_to_str(node.op),
//                 object_type_to_str(value->object_type)
//             );
//     exit(1);
//     return NULL;
// }

// static kod_object_t* eval_binary_op(env_t* env, ast_bin_op_t node) {
//     kod_object_t* left = visit(env, node.left);
//     kod_object_t* right = visit(env, node.right);

//     if (!left || !right) return NULL;

//     if (left->object_type == OBJECT_NUMBER &&
//         right->object_type == OBJECT_NUMBER) {
//             return eval_binary_op_number(left->number, right->number, node.op);
//     }

//     printf("[interpreter]: Error - can't `%s` a `%s` and a `%s`\n",
//                 token_type_to_str(node.op),
//                 object_type_to_str(left->object_type),
//                 object_type_to_str(right->object_type)
//             );
//     exit(1);
//     return NULL;
// }

// static kod_object_t* visit(env_t* env, ast_node_t* node) {
//     switch (node->ast_type) {
//         case AST_ROOT:
//         case AST_BLOCK: {
//             linked_list_node_t* curr = node->ast_compound.head;
//             while (curr) {
//                 kod_object_t* value = visit(env, curr->item);
//                 if (env->does_return) {
//                     return value;
//                 }
//                 curr = curr->next;
//             }
//             break;
//         }

//         case AST_ASSIGNMENT: {
//             kod_object_t* value = visit(env, node->ast_assignment.right);

//             if (node->ast_assignment.left->ast_type != AST_IDENTIFIER) {
//                 printf("[interpreter]: Error - left side is not an identifier\n");
//                 exit(1);
//             }
//             ast_string_t ident = node->ast_assignment.left->ast_string;

//             env_set_variable(env, ident, value);
//             return value;
//         }

//         case AST_IDENTIFIER: {
//             kod_object_t* value = env_get_variable(env, node->ast_string);
//             if (!value) {
//                 printf("[interpreter]: Error - identifer `%.*s` does not exist.\n",
//                     node->ast_string.length,
//                     node->ast_string.value);
//                 exit(1);
//             }
//             return value;
//         }

//         case AST_STRING: {
//             return MAKE_STRING(node->ast_string);
//         }

//         case AST_NUMBER: {
//             return MAKE_NUMBER(node->ast_number.value);
//         }

//         case AST_RETURN_STATEMENT: {
//             env->does_return = true;
//             return visit(env, node->ast_return_statement.value);
//         }

//         case AST_IF_STATEMENT: {
//             kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
//             if (expr->object_type == OBJECT_NUMBER && expr->number) {
//                 expr = visit(env, node->ast_conditional_statement.body);
//             }
//             return expr;
//         }

//         case AST_WHILE_STATEMENT: {
//             kod_object_t* expr = visit(env, node->ast_conditional_statement.expression);
//             while (expr->object_type == OBJECT_NUMBER && expr->number) {
//                 visit(env, node->ast_conditional_statement.body);
                
//                 expr = visit(env, node->ast_conditional_statement.expression);
//             }
//             break;
//         }

//         case AST_UNARY_OP:
//             return eval_unary_op(env, node->ast_unary_op);

//         case AST_BIN_OP:
//             return eval_binary_op(env, node->ast_bin_op);

//         case AST_FUNCTION: {
//             kod_object_t* fn_object = object_new((kod_object_t){
//                 .object_type=OBJECT_FUNCTION,
//                 .function={
//                     .function_node=node->ast_function,
//                     .env=env
//                 }
//             });

//             env_set_variable(env, node->ast_function.name, fn_object);
//             return fn_object;
//         }

//         case AST_ACCESS: {
//             kod_object_t* value = visit(env, node->ast_access.value);
//             if (value) {
//                 return visit(value->attributes, node->ast_access.field);
//             }

//             break;
//         }

//         case AST_CALL: {
//             kod_object_t* fn_object = visit(env, node->ast_call.callable);
//             if (!fn_object) {
//                 printf("[interpreter]: Error - cannot call a null object.\n");
//                 exit(1);
//             }

//             switch (fn_object->object_type) {

//                 case OBJECT_FUNCTION: {
//                     env_t* new_env = env_new(fn_object->function.env);

//                     linked_list_t args = node->ast_call.arguments->ast_compound;
//                     linked_list_t params = fn_object->function.function_node.parameters->ast_compound;

//                     if (args.size != params.size) {
//                         printf("[interpreter]: Error - arguments size does not match parameters size\n");
//                         exit(1);
//                     }

//                     linked_list_node_t* curr_arg = args.head;
//                     linked_list_node_t* curr_param = params.head;
                    
//                     while (curr_arg && curr_param) {
//                         kod_object_t* arg = visit(env, curr_arg->item);
//                         ast_node_t* param = (ast_node_t*)curr_param->item;
//                         if (param->ast_type != AST_IDENTIFIER) {
//                             printf("[interpreter]: Error - param is not an identifer\n");
//                             exit(1);
//                         }

//                         // mapping arg to param
//                         env_set_variable(new_env, param->ast_string, arg);

//                         curr_arg = curr_arg->next;
//                         curr_param = curr_param->next;
//                     }

//                     kod_object_t* value = visit(new_env, fn_object->function.function_node.body);
//                     return value;
//                 }

//                 case OBJECT_NATIVE_FUNCTION: {
//                     // printf("%.*s -> %p\n", fn_object->native_function.name.length, fn_object->native_function.name.value, fn_object->native_function.caller);
//                     return fn_object->native_function.caller(env, node->ast_call.arguments->ast_compound);
//                 }

//                 default: printf("[interpreter]: Error - object from type %s is not callable\n",
//                 object_type_to_str(fn_object->object_type));
//                 exit(1);
//             }
//             break;
//         }
        
//         default:
//             printf("[interpreter]: TODO - visit for ast from type %s is not implemented yet\n",
//                 ast_type_to_str(node->ast_type));
//             exit(1);
//     }
    
//     return NULL;
// }

// kod_object_t* kod_print(env_t* env, linked_list_t params) {
//     linked_list_node_t* curr = params.head;
//     while (curr) {
//         kod_object_t* object = visit(env, curr->item);
//         curr = curr->next;

//         if (!object) {
//             printf("null");
//             continue;
//         }
//         // Switch on the type of the object node.
//         switch (object->object_type) {
//             case OBJECT_NUMBER: {
//                 printf("%g", object->number);
//                 break;
//             }
//             case OBJECT_STRING: {
//                 printf("%.*s", object->string.length, object->string.value);
//                 break;
//             }

//             case OBJECT_FUNCTION: {
//                 printf("<function %.*s at %p>", object->function.function_node.name.length, object->function.function_node.name.value, object);
//                 break;
//             }
//             case OBJECT_TYPE: {
//                 kod_object_t* name = env_get_variable(object->type.attributes, (ast_string_t){.value="name", .length=sizeof("name") - 1});
//                 if (!name || name->object_type != OBJECT_STRING) {
//                     puts("[object]: Error - the type has no name or the name is not a string");
//                     exit(1);
//                 }
//                 printf("%.*s", name->string.length, name->string.value);
//                 break;
//             }
//             case OBJECT_NATIVE_FUNCTION: {
//                 printf("<native function %.*s>", object->native_function.name.length, object->native_function.name.value);
//                 break;
//             }
//             default:
//                 printf("%s", object_type_to_str(object->object_type));
//                 break;
//         }
//         if (curr) printf(" ");
//     }
//     printf("\n");
//     return NULL;
// }

// kod_object_t* kod_time(env_t* env, linked_list_t params) {
//     return MAKE_NUMBER(time(NULL));
// }

// // int fib(int n) {
// //     if (n < 2) return n;
// //     return fib(n - 1) + fib(n - 2);
// // }

// kod_object_t* kod_string_upper(env_t* env, linked_list_t params) {
//     if (!params.size) {
//         puts("[interpreter]: Error - unbound method expected self");
//         exit(1);
//     }
//     // TODO: make not in-place, create a new string, work with that and return it
//     kod_object_t* string = visit(env, params.head->item);
//     for (size_t i = 0; i < string->string.length; ++i) {
//         char* c = string->string.value + i;
//         if (isalpha(*c) && *c & 32) {
//             *c ^= 32;
//         }
//     }
//     return string;
// }

// kod_object_t* eval(ast_node_t* root) {
//     env_init(&string_attributes, NULL);
//     string_attributes.is_global = true;

//     env_init(&int_attributes, NULL);
//     int_attributes.is_global = true;

//     env_t* env = env_new(NULL);

//     MAKE_NATIVE_FN(env, "print", kod_print);
//     MAKE_NATIVE_FN(env, "time", kod_time);

//     // MAKE_NATIVE_TYPE(env, string);
//     env_set_variable( 
//         env, 
//         (ast_string_t){ 
//             .value="str", 
//             .length=sizeof("str") - 1 
//         }, 
//         object_new((kod_object_t){ 
//             .object_type=OBJECT_TYPE, 
//             .type={.attributes=&string_attributes} 
//         })
//     );

//     MAKE_NATIVE_FN(&string_attributes, "upper", kod_string_upper);
//     env_set_variable(
//         &string_attributes,
//         (ast_string_t){.value="name", .length=sizeof("name") - 1},
//         MAKE_STRING(((ast_string_t){.value="str", .length=sizeof("str") - 1}))
//     );
//     time_t start = time(NULL);

//     kod_object_t* value = visit(env, root);
    
//     time_t delta = time(NULL) - start;
//     struct tm* ptm = localtime(&delta);
//     printf("Finished in: %02d:%02d\n",ptm->tm_min, ptm->tm_sec);

//     // start = time(NULL);
//     // for (int i = 1; i < 30; ++i) {
//     //     printf("%d\n", fib(i));
//     // }
//     // delta = time(NULL) - start;
//     // ptm = localtime(&delta);
//     // printf("Finished in: %02d:%02d\n",ptm->tm_min, ptm->tm_sec);
//     return value;
// }