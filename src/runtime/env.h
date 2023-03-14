#pragma once

#include "object.h"
#include "../misc/linked_list.h"
#include <stdbool.h>

typedef struct env_t env_t;

struct env_t {
    env_t* parent;
    linked_list_t locals;
    bool does_return;
};

env_t* env_new(env_t* parent);

kod_object_t* env_get_variable(env_t* env, ast_string_t var_name);
void env_set_variable(env_t* env, ast_string_t var_name, kod_object_t* value);