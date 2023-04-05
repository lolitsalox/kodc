#pragma once

#include "object.h"

typedef struct Kod_NumberMethods {
    binaryfunc  kod_add;
    binaryfunc  kod_sub;
    binaryfunc  kod_mul;
    binaryfunc  kod_div;
    binaryfunc  kod_mod;
    binaryfunc  kod_pow;
    unaryfunc   kod_neg;
    unaryfunc   kod_pos;
    unaryfunc   kod_abs;
    boolfunc    kod_bool;
    unaryfunc   kod_invert;
    binaryfunc   kod_shl;
    binaryfunc   kod_shr;
    binaryfunc   kod_xor;
    binaryfunc   kod_or;
    binaryfunc   kod_and;
    unaryfunc   kod_int;
    unaryfunc   kod_float;
    binaryfunc   kod_floor;
} Kod_NumberMethods;

typedef struct KodTypeObject {
    KodObject object;
    char* type_name;
    Kod_MethodDef* methods;
    Kod_NumberMethods* as_number;
    hashfunc hash;
    methfunc call;
    methfunc repr;
    typefunc new;
    bool (*free) (KodObject*);
    char* doc;
    size_t size;
    // as_sequence ?
    // as_mapping ? 
    
} KodTypeObject;

extern KodTypeObject KodType_Type;
extern KodTypeObject KodType_Null;
extern KodObject Kod_Null;

#define As_Type(a) ((KodTypeObject*)a)->type_object
#define Is(a, b) (a == b)
#define Is_Type(a, type) (a && ((KodObject*)a)->type_object == type)
#define Is_Null(a) (Is(a, &Kod_Null))
#define Is_Number(a) (As_Type(a)->as_number != NULL)

KodObject* null_new();