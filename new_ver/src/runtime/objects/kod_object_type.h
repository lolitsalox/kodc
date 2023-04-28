#pragma once

#include "kod_object.h"

#define TYPE_HEADER(name) .base={.kind=OBJECT_TYPE,.ref_count=1,.type=&KodType_Type},.tp_name=name,

typedef struct KodObjectNumberMethods {
    binary_func bit_xor;
    binary_func bit_and;
    binary_func bit_or;
    binary_func bit_shl;
    binary_func bit_shr;

    binary_func add;
    binary_func sub;
    binary_func mul;
    binary_func div;
    binary_func mod;
    binary_func pow;

    binary_func bool_or;
    binary_func bool_and;
    binary_func gt;
    binary_func gte;
    binary_func lt;
    binary_func lte;

    binary_func eq;
    binary_func ne;

    unary_func u_not;
    unary_func u_sub;
    unary_func u_add;
    unary_func u_neg;

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
    normal_func call;
    kod_func free;
} KodObjectType;

extern KodObjectType KodType_Type;

// Status kod_type_init(char* tp_name, KodObjectType* out);
// Status kod_type_new(char* tp_name, KodObjectType** out);