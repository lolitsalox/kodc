#include "env.h"
#include "object.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct kod_object_pair_t {
    char* name;
    kod_object_t* object;
} kod_object_pair_t;

void env_init(env_t* env, env_t *parent) {
    env->parent = parent;
    env->does_return = false;
    env->is_global = false;
    linked_list_init(&env->locals);
}

env_t *env_new(env_t *parent) {
    env_t* env = malloc(sizeof(env_t));
    env_init(env, parent);
    return env;
}

kod_object_t* env_get_variable(env_t* env, char* var_name) {
    if (!env) return NULL;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;

        if (strcmp(pair->name, var_name) == 0) {
            return pair->object;
        }

        curr = curr->next;
    }
    
    return env_get_variable(env->parent, var_name);
}

void env_set_variable(env_t* env, char* var_name, kod_object_t* value) {
    if (!env) return;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;

        if (strcmp(pair->name, var_name) == 0) {
            object_dec_ref(pair->object);
            object_inc_ref(value);
            pair->object = value;
            return;
        }

        curr = curr->next;
    }

    object_inc_ref(value);
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
        printf("%s ", pair->name);

        object_print(pair->object, 0);

        curr = curr->next;
    }
}

void env_free(env_t* env) {
    if (!env) return;

    linked_list_node_t* curr = env->locals.head;

    while (curr) {
        kod_object_pair_t* pair = (kod_object_pair_t*)curr->item;
        object_dec_ref(pair->object);
        free(pair->name);
        free(pair);
        linked_list_node_t* next = curr->next;
        free(curr);
        curr = next;
    }
    free(env);
}