#include "kod_object_type.h"

static Status type_str_impl(KodObjectType* self, KodObject** out) {
    RETURN_STATUS_OK
}

static Status type_str(KodObject* self, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("self is null")
    if (self->kind != OBJECT_TYPE) RETURN_STATUS_FAIL("self is not from kind 'type'");

    return type_str_impl((KodObjectType*)self, out);
}

KodObjectType KodObjectType_Type = {
    TYPE_HEADER(KodObjectType_Type)
    .tp_name="type",
    .as_number=0,
    .as_subscript=0,
    .str=type_str,
    .free=0,
};
