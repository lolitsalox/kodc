#pragma once

#include "objects/object.h"

typedef struct NativeFunctionPair {
    char* name;
    builtinfunc func;
} NativeFunctionPair; 

KodObject* native_print(KodObject* args, KodObject* kwargs);

static NativeFunctionPair native_functions[] = {
    {"print", native_print},
};