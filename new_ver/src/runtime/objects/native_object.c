#include "native_object.h"

Status kod_object_new_native_func(native_func func, char* name, KodObjectNativeFunc** out) {
    KodObjectNativeFunc* obj = malloc(sizeof(KodObjectNativeFunc));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for native func object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_func = func;
    obj->name = name;
    obj->base.kind = OBJECT_NATIVE_FUNC;
    obj->base.type = &KodType_NativeFunc;
    obj->base.ref_count = 0;
    obj->base.attributes = NULL;

    *out = obj;
    RETURN_STATUS_OK
}

Status kod_object_new_native_method(normal_func method, char* name, KodObjectNativeMethod** out) {
    KodObjectNativeMethod* obj = malloc(sizeof(KodObjectNativeMethod));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for native method object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_method = method;
    obj->name = name;
    obj->base.kind = OBJECT_NATIVE_METHOD;
    obj->base.type = &KodType_NativeMethod;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status func_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    char* buffer = malloc(32);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");

    snprintf(buffer, 32, "<built in function %s>", ((KodObjectNativeFunc*)self)->name);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status method_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    char* buffer = malloc(32);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");

    snprintf(buffer, 32, "<built in method %s>", ((KodObjectNativeMethod*)self)->name);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status func_call(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return ((KodObjectNativeFunc*)self)->_func(vm, args, kwargs, out);
}

static Status method_call(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return ((KodObjectNativeMethod*)self)->_method(vm, self, args, kwargs, out);
}

KodObjectType KodType_NativeFunc = {
    TYPE_HEADER("native function")
    .as_number = 0,
    .as_subscript = 0,
    .repr = func_str,
    .str = func_str,
    .hash = 0,
    .call = func_call,
    .free = kod_object_free
};

KodObjectType KodType_NativeMethod = {
    TYPE_HEADER("native method")
    .as_number = 0,
    .as_subscript = 0,
    .repr = method_str,
    .str = method_str,
    .hash = 0,
    .call = method_call,
    .free = kod_object_free
};