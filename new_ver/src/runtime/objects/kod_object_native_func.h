#pragma once

#include "kod_object_type.h"

typedef struct KodObjectNativeFunc {
    KodObject base;
    normal_func _func;
    char* name;
} KodObjectNativeFunc;

extern KodObjectType KodType_NativeFunc;

Status kod_object_new_native_func(normal_func func, char* name, KodObjectNativeFunc** out);