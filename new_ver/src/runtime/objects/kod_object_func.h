#pragma once

#include "kod_object_type.h"
#include <compiler/compiler.h>

typedef struct KodObjectFunc {
    KodObject base;
    Code _code;
} KodObjectFunc;

extern KodObjectType KodType_Func;

Status kod_object_new_func(Code code, KodObjectFunc** out);