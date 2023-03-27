#pragma once

#include "../misc/linked_list.h"
#include <string.h>

typedef struct Environment Environment;
typedef struct Kod_Object Kod_Object;

typedef struct ObjectNamePair {
    char* name;
    Kod_Object* object;
} ObjectNamePair;

void free_object_name_pair(ObjectNamePair object_name_pair);

typedef struct ObjectNamePairNode {
    ObjectNamePair object_name_pair;
    struct ObjectNamePairNode* next;
} ObjectNamePairNode;

ObjectNamePairNode* new_object_name_pair_node();
void init_object_name_pair_node(ObjectNamePairNode* object_name_pair_node);
void free_object_name_pair_node(ObjectNamePairNode* object_name_pair_node);

typedef struct Environment {
    ObjectNamePairNode* head;
    ObjectNamePairNode* tail;
} Environment;

void init_environment(Environment* env);

Environment* new_environment();

Kod_Object* get_environment(Environment* env, char* name);
void set_environment(Environment* env, ObjectNamePair pair);
void update_environment(Environment* env, Environment* other);
void ref_environment(Environment* env);

void print_environment(Environment* env);
void free_environment(Environment* env);