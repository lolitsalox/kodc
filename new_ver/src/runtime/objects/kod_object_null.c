#include "kod_object_null.h"

static Status null_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = strdup("null");
    RETURN_STATUS_OK
}

Status null_int(KodObject* self, i64* out) {
    *out = 0;
    RETURN_STATUS_OK
}

static Status null_float(KodObject* self, f64* out) {
    *out = 0;
    RETURN_STATUS_OK
}

static Status null_bool(KodObject* self, bool* out) {
    *out = false;
    RETURN_STATUS_OK
}

KodObjectNumberMethods null_as_number = {
    .add=0,
    .sub=0,
    .mul=0,
    .div=0,

    ._int=null_int,
    ._float=null_float,
    ._bool=null_bool,

};

KodObjectType KodType_Null = {
    TYPE_HEADER("null")
    .as_number=&null_as_number,
    .as_subscript=0,
    .str=null_str,
    .hash=0,
    .call=0,
    .free=0
};

KodObjectNull KodObject_Null = {
    .base={.kind=OBJECT_NULL,
    .ref_count=1,
    .type=&KodType_Null}
};

Status kod_object_new_null(KodObjectNull** out) {
    *out = &KodObject_Null;
    RETURN_STATUS_OK
}