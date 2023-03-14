#pragma once

#include "env.h"
#include <stdint.h>
#include "../parser/ast.h"

typedef struct kod_object_t kod_object_t;

typedef enum kod_object_type_t {
    OBJECT_NONE,
    OBJECT_NUMBER,
    OBJECT_STRING,
    OBJECT_FUNCTION,
    OBJECT_NATIVE_FUNCTION,
    OBJECT_TYPE,
} kod_object_type_t;

typedef struct kod_function_t {
    ast_function_t function_node;
    struct env_t* env;
} kod_function_t;


typedef struct kod_native_function_t {
    ast_string_t name;
    kod_object_t* (*caller)(struct env_t* env, linked_list_t params);
} kod_native_function_t;

typedef struct kod_type_t {
    struct env_t* attributes;
} kod_type_t;

struct kod_object_t {
    kod_object_type_t object_type;

    union {
        double number;
        ast_string_t string;
        kod_function_t function;
        kod_native_function_t native_function;
        kod_type_t type;
    };
};

const char* object_type_to_str(kod_object_type_t type);
void object_print(kod_object_t* object, uint32_t indent_level);
kod_object_t* object_new(kod_object_t object);