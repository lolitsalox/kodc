#include "kod_builtins.h"

#include "object.h"
#include "env.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static env_t global_string_attributes;
static env_t global_int_attributes;

extern kod_object_t* visit(env_t* env, ast_node_t* node);

kod_object_t* kod_builtin_print(env_t* env, linked_list_t params) {
    linked_list_node_t* curr = params.head;
    while (curr) {
        kod_object_t* object = visit(env, curr->item);
        curr = curr->next;

        if (!object) {
            printf("null");
            continue;
        }
        // Switch on the type of the object node.
        switch (object->object_type) {
            case OBJECT_INT: {
                printf("%lld", object->_int);
                break;
            }
            case OBJECT_FLOAT: {
                printf("%f", object->_float);
                break;
            }
            case OBJECT_STRING: {
                printf("%s", object->_string);
                break;
            }

            case OBJECT_FUNCTION: {
                printf("<function %.*s at %p>", object->_function.function_node.name.length, object->_function.function_node.name.value, object);
                break;
            }
            case OBJECT_NATIVE_FUNCTION: {
                printf("<native function %.*s>", object->_native_function.name.length, object->_native_function.name.value);
                break;
            }
            case OBJECT_TYPE: {
                kod_object_t* name = env_get_variable(object->attributes, "name");

                if (!name || name->object_type != OBJECT_STRING) {
                    puts("[object]: Error - name is not a string or name is null");
                    exit(1);
                }
                printf("<type %s at %p>", name->_string, object);
                break;
            }
            default:
                printf("%s", object_type_to_str(object->object_type));
                break;
        }
        if (curr) printf(" ");

        if (object->from_return)
            object_dec_ref(object);
    }
    printf("\n");
    return NULL;
}

kod_object_t* kod_builtin_str_upper(env_t* env, linked_list_t params) {
    linked_list_node_t* curr = params.head;
    if (!curr) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* this = visit(env, curr->item);
    if (!this) {
        printf("this is null!!!\n");
        exit(1);
    }

    size_t str_size = strlen(this->_string) + 1;
    char* str = malloc(str_size);
    strncpy(str, this->_string, str_size);

    kod_object_t* new_object = make_string(str);
    strupr(new_object->_string);
    return new_object;
}

kod_object_t* kod_builtin_int_add(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;

    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }


    kod_object_t* left = visit(env, curr->item);
    if (left->object_type == OBJECT_TYPE) {
        object_dec_ref(left);
        curr = curr->next;
        left = visit(env, curr->item);
    }
    
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int + right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_sub(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int - right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_gt(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int > right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_lt(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int < right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_mul(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int * right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_shl(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int << right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_shr(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int >> right->_int);
    return new_object;
}

kod_object_t* kod_builtin_int_and(env_t* env, linked_list_t params) {
    if (params.size < 2) {
        printf("Not enough args\n");
        exit(1);
    }

    linked_list_node_t* curr = params.head;
    if (!curr || !curr->next) {
        printf("no args!!\n");
        exit(1);
    }

    kod_object_t* left = visit(env, curr->item);
    kod_object_t* right = visit(env, curr->next->item);

    if (!left || !right) {
        printf("left or right are null!!!\n");
        exit(1);
    }

    if (right->object_type != OBJECT_INT) {
        printf("can't add from this type to an int\n");
        exit(1);
    }

    kod_object_t* new_object = make_int(left->_int & right->_int);
    return new_object;
}

void make_native_fn(env_t* global_env, char* name, void* fn) {
    const size_t str_size = sizeof(name);

    char* fn_name = malloc(str_size);
    strncpy(fn_name, name, str_size);

    env_set_variable( 
        global_env, 
        fn_name, 
        object_new((kod_object_t){ 
            .object_type=OBJECT_NATIVE_FUNCTION, 
            ._native_function={.caller=fn,.name={.value=name,.length=str_size - 1}},
            .attributes=NULL, 
        })
    );
}

kod_object_t* make_int(int64_t val) {
    return object_new((kod_object_t){
        .object_type=OBJECT_INT,
        ._int=val,
        .attributes=&global_int_attributes,
    });
}

kod_object_t* make_string(char* val) {
    return object_new((kod_object_t){
        .object_type=OBJECT_STRING,
        ._string=val,
        .attributes=&global_string_attributes,
    });
}

void init_string(env_t* global_env) {
    env_t* type_str_attributes = env_new(global_env);

    env_init(&global_string_attributes, NULL);
    global_string_attributes.is_global = true;

    size_t name_size = sizeof("str");
    char* type_name = malloc(name_size);
    strncpy(type_name, "str", name_size);

    env_set_variable( 
        global_env, 
        type_name, 
        object_new((kod_object_t){ 
            .object_type=OBJECT_TYPE, 
            .attributes=type_str_attributes, 
        })
    );

    name_size = sizeof("__name__");
    char* field_name = malloc(name_size);
    strncpy(field_name, "__name__", name_size);

    env_set_variable( 
        type_str_attributes, 
        field_name, 
        make_string(type_name)
    );

    make_native_fn(&global_string_attributes, "upper", kod_builtin_str_upper);
}

void init_int(env_t* global_env) {
    env_t* type_int_attributes = env_new(global_env);

    env_init(&global_int_attributes, NULL);
    global_int_attributes.is_global = true;

    size_t name_size = sizeof("int");
    char* type_name = malloc(name_size);
    strncpy(type_name, "int", name_size);

    env_set_variable( 
        global_env, 
        type_name, 
        object_new((kod_object_t){ 
            .object_type=OBJECT_TYPE, 
            .attributes=type_int_attributes, 
        })
    );

    name_size = sizeof("__name__");
    char* field_name = malloc(name_size);
    strncpy(field_name, "__name__", name_size);

    env_set_variable( 
        type_int_attributes, 
        field_name, 
        make_string(type_name)
    );

    make_native_fn(type_int_attributes, "__add__", kod_builtin_int_add);

    make_native_fn(&global_int_attributes, "__add__", kod_builtin_int_add);
    make_native_fn(&global_int_attributes, "__sub__", kod_builtin_int_sub);
    make_native_fn(&global_int_attributes, "__gt__", kod_builtin_int_gt);
    make_native_fn(&global_int_attributes, "__lt__", kod_builtin_int_lt);
    make_native_fn(&global_int_attributes, "__mul__", kod_builtin_int_mul);
    make_native_fn(&global_int_attributes, "__shl__", kod_builtin_int_shl);
    make_native_fn(&global_int_attributes, "__shr__", kod_builtin_int_shr);
    make_native_fn(&global_int_attributes, "__and__", kod_builtin_int_and);
}


void builtins_init(env_t* global_env) {
    init_string(global_env);
    init_int(global_env);
    
    make_native_fn(global_env, "print", kod_builtin_print);
}