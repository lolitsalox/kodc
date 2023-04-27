#include "kod_object_type.h"

static Status type_str_impl(KodObjectType* self, char** out) {
    if (!out) RETURN_STATUS_FAIL("strdup failed")

    char* buffer = malloc(32);
    snprintf(buffer, 32, "<type '%s'>", self->tp_name);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status type_str(KodObject* self, char** out) {
    if (!out) RETURN_STATUS_FAIL("out is null")
    if (!self) RETURN_STATUS_FAIL("self is null")
    if (self->kind != OBJECT_TYPE) RETURN_STATUS_FAIL("self is not from kind 'type'");

    return type_str_impl((KodObjectType*)self, out);
}

KodObjectType KodType_Type = {
    TYPE_HEADER("type")
    .as_number=0,
    .as_subscript=0,
    .str=type_str,
    .hash=0,
    .call=0,
    .free=0,
};
