#pragma once

#include "../compiler/compiler.h"
#include "env.h"

enum Kod_ObjectType {
    OBJECT_NULL,
    OBJECT_BOOL,
    OBJECT_INTEGER,
    OBJECT_FLOAT,
    OBJECT_STRING,
    OBJECT_CODE,
    OBJECT_NATIVE_FUNCTION,
};

String object_type_to_str(enum Kod_ObjectType type);

typedef struct Kod_Object Kod_Object;
typedef Kod_Object* (*NativeFunction) (Kod_Object** args, size_t size);

typedef struct Kod_NativeFunction {
    String name;
    NativeFunction callable;
} Kod_NativeFunction;

struct Kod_Object {
    enum Kod_ObjectType type;
    Environment* attributes;
    int32_t ref_count;
    union {
        bool _bool;
        int64_t _int;
        double _float;
        String _string;
        Code _code;
        Kod_NativeFunction _function;
    };
};

void init_native_attributes();
void init_native_functions();

Environment* get_native_functions();

Kod_Object* new_object(enum Kod_ObjectType type, Environment* attributes);
Kod_Object* new_null_object();
void free_object(Kod_Object* object);

void ref_object(Kod_Object* object);

bool deref_object(Kod_Object* object);

Kod_Object* new_null_object();
Kod_Object* new_bool_object(bool value);
Kod_Object* new_int_object(int64_t value);
Kod_Object* new_string_object(char* value);
Kod_Object* new_code_object(Code value);
Kod_Object* new_native_function_object(char* name, NativeFunction callable);