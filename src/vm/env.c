#include "env.h"

#include <stdio.h>
#include <stdlib.h>

#include "object.h"

void free_object_name_pair(ObjectNamePair object_name_pair) {
    if (object_name_pair.name)
        free(object_name_pair.name);

    if (object_name_pair.object)
        deref_object(object_name_pair.object);
}

ObjectNamePairNode* new_object_name_pair_node() {
    ObjectNamePairNode* object_name_pair_node = malloc(sizeof(ObjectNamePairNode));
    init_object_name_pair_node(object_name_pair_node);
    return object_name_pair_node;
}

void init_object_name_pair_node(ObjectNamePairNode* object_name_pair_node) {
    if (!object_name_pair_node) return;
    object_name_pair_node->object_name_pair = (ObjectNamePair){0};
    object_name_pair_node->next = NULL;
}

void free_object_name_pair_node(ObjectNamePairNode* object_name_pair_node) {
    if (!object_name_pair_node) return;
    free_object_name_pair(object_name_pair_node->object_name_pair);
    free(object_name_pair_node);
}

void init_environment(Environment* env) {
    if (!env) return;
    env->head = NULL;
    env->tail = NULL;
}

Environment* new_environment() {
    Environment* env = malloc(sizeof(Environment));
    init_environment(env);
    return env;
}

Kod_Object* get_environment(Environment* env, char* name) {
    ObjectNamePairNode* curr_pair = env->head;
    while (curr_pair) {
        if (strcmp(curr_pair->object_name_pair.name, name) == 0) {
            return curr_pair->object_name_pair.object;
        }
        curr_pair = curr_pair->next;
    }
    return NULL;
}

void set_environment(Environment* env, ObjectNamePair pair) {
    ObjectNamePairNode* curr_pair = env->head;
    ref_object(pair.object);

    if (!curr_pair) {
        env->head = new_object_name_pair_node();
        env->head->object_name_pair.object = pair.object;
        size_t size = strlen(pair.name) + 1;
        env->head->object_name_pair.name = malloc(size);
        strncpy(env->head->object_name_pair.name, pair.name, size);

        env->tail = env->head;
        return;
    }
    
    while (curr_pair) {
        if (strcmp(curr_pair->object_name_pair.name, pair.name) == 0) {
            deref_object(curr_pair->object_name_pair.object);
            curr_pair->object_name_pair.object = pair.object;
            return;
        }
        curr_pair = curr_pair->next;
    }

    env->tail->next = new_object_name_pair_node();
    env->tail = env->tail->next; // replace with new tail

    env->tail->object_name_pair.object = pair.object;
    size_t size = strlen(pair.name) + 1;
    env->tail->object_name_pair.name = malloc(size);
    strncpy(env->tail->object_name_pair.name, pair.name, size);
}

void update_environment(Environment* env, Environment* other) {
    if (!other || !env) return;
    ObjectNamePairNode* curr_pair = other->head;
    while (curr_pair) {
        set_environment(env, curr_pair->object_name_pair);
        curr_pair = curr_pair->next;
    }
}

void print_environment(Environment* env) {
    ObjectNamePairNode* curr_pair = env->head;
    while (curr_pair) {
        printf("%s: %s\n", curr_pair->object_name_pair.name, object_type_to_str(curr_pair->object_name_pair.object->type));
        curr_pair = curr_pair->next;
    }
}

void free_environment(Environment* env) {
    if (!env) return;
    ObjectNamePairNode* curr_pair = env->head;
    ObjectNamePairNode* next = NULL;
    while (curr_pair) {
        next = curr_pair->next;
        free_object_name_pair_node(curr_pair);
        curr_pair = next;
    }
}