#pragma once

#include "../parser/ast.h"
#include "../misc/linked_list.h"
#include <stdbool.h>

typedef struct env_t env_t;
typedef struct kod_object_t kod_object_t;

struct env_t {
    env_t* parent;
    linked_list_t locals;
    bool does_return;
};

void env_init(env_t* env, env_t* parent);

env_t* env_new(env_t* parent);

kod_object_t* env_get_variable(env_t* env, ast_string_t var_name);
void env_set_variable(env_t* env, ast_string_t var_name, kod_object_t* value);
void env_print(env_t* env);