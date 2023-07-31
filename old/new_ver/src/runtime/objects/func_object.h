#pragma once

#include "type_object.h"
#include "../../compiler/compiler.h"

typedef struct KodObjectFunc {
    KodObject base;
    Code _code;
} KodObjectFunc;

extern KodObjectType KodType_Func;

Status kod_object_new_func(Code code, KodObjectFunc** out);