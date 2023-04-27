#pragma once

#include "kod_object.h"

#define TYPE_HEADER(name) .base={.kind=OBJECT_TYPE,.ref_count=1,.type=&KodType_Type},.tp_name=name,

typedef struct KodObjectNumberMethods {
    binary_func add;
    binary_func sub;
    binary_func mul;
    binary_func div;

    Status (*_bool)(KodObject*, bool* out);
    Status (*_int)(KodObject*, i64* out);
    Status (*_float)(KodObject*, f64* out);
} KodObjectNumberMethods;

typedef struct KodObjectSubscriptMethods {
    method_func get;
    method_func set;
} KodObjectSubscriptMethods;

typedef struct KodObjectType {
    KodObject base;
    char* tp_name;
    KodObjectNumberMethods* as_number;
    KodObjectSubscriptMethods* as_subscript;
    Status (*str)(KodObject* self, char** out);
    unary_func hash;
    full_func call;
    kod_func free;
} KodObjectType;

extern KodObjectType KodType_Type;

// Status kod_type_init(char* tp_name, KodObjectType* out);
// Status kod_type_new(char* tp_name, KodObjectType** out);