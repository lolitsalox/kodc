// #include "env.h"
// #include "object.h"
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>


// typedef struct ObjectPair {
//     char* name;
//     Object* object;
// } ObjectPair;

// void free_object_pair(ObjectPair* pair) {
//     free_object(pair->object);
//     free(pair->name);
//     free(pair);
// }

// Environment init_env(Environment* env) {
//     return (Environment){
//         .locals={0}
//     };
// }

// Environment *new_env() {
//     Environment* env = malloc(sizeof(Environment));
//     *env = init_env();
//     return env;
// }

// Object* env_get_variable(Environment* env, char* var_name) {
//     if (!env) return NULL;

//     linked_list_node_t* curr = env->locals.head;

//     while (curr) {
//         ObjectPair* pair = (ObjectPair*)curr->item;

//         if (strcmp(pair->name, var_name) == 0) {
//             return pair->object;
//         }

//         curr = curr->next;
//     }
    
//     return env_get_variable(env->parent, var_name);
// }

// void env_set_variable(Environment* env, char* var_name, Object* value) {
//     if (!env) return;

//     linked_list_node_t* curr = env->locals.head;

//     while (curr) {
//         ObjectPair* pair = (ObjectPair*)curr->item;

//         if (strcmp(pair->name, var_name) == 0) {
//             deref_object(pair->object);
//             ref_object(value);
//             pair->object = value;
//             return;
//         }

//         curr = curr->next;
//     }

//     ref_object(value);
//     ObjectPair* pair = malloc(sizeof(ObjectPair));
//     pair->name = var_name;
//     pair->object = value;
    
//     linked_list_append(&env->locals, pair);
// }

// void print_env(Environment *env) {
//     if (!env) return;

//     linked_list_node_t* curr = env->locals.head;

//     while (curr) {
//         ObjectPair* pair = (ObjectPair*)curr->item;
//         printf("%s ", pair->name);

//         print_object(pair->object, 0);

//         curr = curr->next;
//     }
// }

// bool delete_env(Environment* env) {
//     if (!env) return false;
//     linked_list_node_t* curr = env->locals.head;

//     while (curr) {
//         ObjectPair* pair = (ObjectPair*)curr->item;
//         free_object_pair(pair);
//         linked_list_node_t* next = curr->next;
//         free(curr);
//         curr = next;
//     }
//     return true;
// }

// void free_env(Environment* env) {
//     if (delete_env(env)) free(env);
// }

// /*

// n = 0

// f() {
//     return "Hello"
// }

// n = f()

// */