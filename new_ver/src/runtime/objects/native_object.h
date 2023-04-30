#pragma once

#include "type_object.h"

#define STRUCT_BUILTIN_FUNC(builtin_name) {.base={.kind=OBJECT_NATIVE_FUNC, .ref_count=1, .type=&KodType_NativeFunc}, .name=#builtin_name, ._func=native_ ## builtin_name},
#define STRUCT_BUILTIN_METHOD(builtin_name) {.base={.kind=OBJECT_NATIVE_METHOD, .ref_count=1, .type=&KodType_NativeMethod}, .name=#builtin_name, ._method=native_ ## builtin_name},

typedef struct KodObjectNativeFunc {
    KodObject base;
    native_func _func;
    char* name;
} KodObjectNativeFunc;

extern KodObjectType KodType_NativeFunc;

Status kod_object_new_native_func(native_func func, char* name, KodObjectNativeFunc** out);

typedef struct KodObjectNativeMethod {
    KodObject base;
    normal_func _method;
    char* name;
} KodObjectNativeMethod;

extern KodObjectType KodType_NativeMethod;

Status kod_object_new_native_method(normal_func method, char* name, KodObjectNativeMethod** out);