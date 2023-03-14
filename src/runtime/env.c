#include "env.h"

#include <stdlib.h>

typedef struct kod_object_pair_t {
    ast_string_t name;
    kod_object_t* object;
} kod_object_pair_t;

env_t* env_new(env_t* parent) {
    env_t* env = malloc(sizeof(env_t));
    env->parent = parent;
    env->does_return = false;
    linked_list_init(&env->locals);
    return env;
}

kod_object_t* env_get_variable(env_t* env, ast_string_t var_name) {
    if (!env) return NULL;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;

        if (ast_string_compare(pair->name, var_name) == 0) {
            return pair->object;
        }

        curr = curr->next;
    }
    
    return env_get_variable(env->parent, var_name);
}

void env_set_variable(env_t* env, ast_string_t var_name, kod_object_t* value) {
    kod_object_pair_t* pair = malloc(sizeof(kod_object_pair_t));
    pair->name = var_name;
    pair->object = value;
    
    linked_list_append(&env->locals, pair);
}
