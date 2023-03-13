#pragma once

// * NOT USED

#include <stdint.h>
#include "../parser/ast.h"

typedef enum kod_object_type_t {
    OBJECT_NONE,
    OBJECT_NUMBER,
    OBJECT_STRING,
    OBJECT_FUNCTION,
} kod_object_type_t;

typedef struct kod_number_t {
    double value;
} kod_number_t;

typedef struct kod_string_t {
    char* value;
    uint32_t length;
} kod_string_t;

typedef struct kod_function_t {
    ast_function_t function;
} kod_function_t;

typedef struct kod_object_t {
    kod_object_type_t object_type;

    union {
        kod_number_t number;
        kod_string_t string;
        kod_function_t function;
    };
} kod_object_t;

const char* object_type_to_str(kod_object_type_t type);
void object_print(const kod_object_t* object, uint32_t indent_level);
kod_object_t* object_new(kod_object_t object);