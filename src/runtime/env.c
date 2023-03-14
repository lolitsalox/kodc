#include "env.h"
#include "object.h"
#include <stdlib.h>
#include <stdio.h>


typedef struct kod_object_pair_t {
    ast_string_t name;
    kod_object_t* object;
} kod_object_pair_t;

void env_init(env_t* env, env_t *parent) {
    env->parent = parent;
    env->does_return = false;
    linked_list_init(&env->locals);
}

env_t *env_new(env_t *parent) {
    env_t* env = malloc(sizeof(env_t));
    env_init(env, parent);
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
    if (!env) return;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;

        if (ast_string_compare(pair->name, var_name) == 0) {
            pair->object = value;
            return;
        }

        curr = curr->next;
    }

    kod_object_pair_t* pair = malloc(sizeof(kod_object_pair_t));
    pair->name = var_name;
    pair->object = value;
    
    linked_list_append(&env->locals, pair);
}

void env_print(env_t *env) {
    if (!env) return;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;
        printf("%.*s ", pair->name.length, pair->name.value);

        object_print(pair->object, 0);

        curr = curr->next;
    }
}
