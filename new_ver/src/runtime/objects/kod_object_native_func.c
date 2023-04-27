#include "kod_object_native_func.h"

Status kod_object_new_native_func(normal_func func, char* name, KodObjectNativeFunc** out) {
    KodObjectNativeFunc* obj = malloc(sizeof(KodObjectNativeFunc));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for native func object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_func = func;
    obj->name = name;
    obj->base.kind = OBJECT_NATIVE_FUNC;
    obj->base.type = &KodType_NativeFunc;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status func_str_impl(KodObjectNativeFunc* self, char** out) {
    char* buffer = malloc(32);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");
    
    snprintf(buffer, 32, "<built in function %s>", self->name);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status func_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return func_str_impl((KodObjectNativeFunc*)self, out);
}

static Status func_call(KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    
    return ((KodObjectNativeFunc*)self)->_func(args, kwargs, out);
}

KodObjectType KodType_NativeFunc = {
    TYPE_HEADER("native function")
    .as_number=0,
    .as_subscript=0,
    .str=func_str,
    .hash=0,
    .call=func_call,
    .free=kod_object_free
};