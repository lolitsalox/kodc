#pragma once

#include "kod_object_type.h"

typedef struct KodObjectNativeFunc {
    KodObject base;
    native_func _func;
    char* name;
} KodObjectNativeFunc;

extern KodObjectType KodType_NativeFunc;

Status kod_object_new_native_func(native_func func, char* name, KodObjectNativeFunc** out);