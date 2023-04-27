#include "kod_object_func.h"

Status kod_object_new_func(Code code, KodObjectFunc** out) {
    KodObjectFunc* obj = malloc(sizeof(KodObjectFunc));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for func object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_code = code;
    obj->base.kind = OBJECT_FUNC;
    obj->base.type = &KodType_Func;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status func_str_impl(KodObjectFunc* self, char** out) {
    char* buffer = malloc(40);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");
    
    snprintf(buffer, 40, "<function %s at 0x%p>", self->_code.name, self);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status func_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return func_str_impl((KodObjectFunc*)self, out);
}

KodObjectType KodType_Func = {
    TYPE_HEADER("function")
    .as_number=0,
    .as_subscript=0,
    .str=func_str,
    .hash=0,
    .call=0,
    .free=kod_object_free
};