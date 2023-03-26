// #pragma once

// #include "../misc/linked_list.h"

// typedef struct Environment Environment;
// typedef struct Object Object;

// typedef struct ObjectNamePair {
//     char* name;
//     Object object;
// } ObjectNamePair;

// bool delete_object_name_pair(ObjectNamePair* object_name_pair);

// typedef struct ObjectNamePairNode {
//     ObjectNamePair object_name_pair;
//     struct ObjectNamePairNode* next;
// } ObjectNamePairNode;

// bool delete_object_name_pair_node(ObjectNamePairNode* object_name_pair_node);
// void free_object_name_pair_node(ObjectNamePairNode* object_name_pair_node);

// struct Environment {
//     ObjectNamePairNode* pairs;
// };

// Environment init_env(Environment* env);

// Environment* new_env();

// Object* env_get_variable(Environment* env, char* var_name);
// void env_set_variable(Environment* env, char* var_name, Object* value);
// void print_env(Environment* env);

// bool delete_env(Environment* env);
// void free_env(Environment* env);