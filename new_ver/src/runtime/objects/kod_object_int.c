#include "kod_object_int.h"
#include "kod_object_float.h"
#include "kod_object_bool.h"

static inline i64 i64_add(i64 a, i64 b) { return a + b; }

static inline i64 i64_sub(i64 a, i64 b) { return a - b; }

static inline i64 i64_mul(i64 a, i64 b) { return a * b; }

Status kod_object_new_int(i64 value, KodObjectInt** out) {
    KodObjectInt* obj = malloc(sizeof(KodObjectInt));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for int object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_int = value;
    obj->base.kind = OBJECT_INT;
    obj->base.type = &KodType_Int;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status int_str_impl(KodObjectInt* self, char** out) {
    char* buffer = malloc(32);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");
    _ltoa_s((long)self->_int, buffer, 32, 10);
    
    *out = buffer;
    RETURN_STATUS_OK
}

Status int_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return int_str_impl((KodObjectInt*)self, out);
}

static Status int_bin(KodObject* self, KodObject* other, KodObject** out, i64 (*op)(i64 a, i64 b)) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!self->type->as_number) RETURN_STATUS_FAIL("self Type has no number representation")
    if (!other->type->as_number) RETURN_STATUS_FAIL("other Type has no number representation")

    if (self->type->as_number->_int && other->type->as_number->_int) {
        i64 left = 0;
        Status s = self->type->as_number->_int(self, (i64*)&left);
        if (s.type == ST_FAIL) return s;

        i64 right = 0;
        s = other->type->as_number->_int(other, (i64*)&right);
        if (s.type == ST_FAIL) return s;
        
        if ((s = kod_object_new_int(op(left, right), (KodObjectInt**)out)).type == ST_FAIL) return s;
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Type has no int representation")
}

Status int_add(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT) return other->type->as_number->add(self, other, out);
    return int_bin(self, other, out, i64_add);
}

Status int_sub(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT) return other->type->as_number->sub(self, other, out);
    return int_bin(self, other, out, i64_sub);
}

Status int_mul(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT) return other->type->as_number->mul(self, other, out);
    return int_bin(self, other, out, i64_mul);
}

Status int_div(KodObject* self, KodObject* other, KodObject** out) {
    return KodType_Float.as_number->div(self, other, out);
}

Status int_gt(KodObject* self, KodObject* other, KodObject** out) {
    return KodType_Bool.as_number->gt(self, other, out);
}

Status int_int(KodObject* self, i64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = ((KodObjectInt*)self)->_int;
    RETURN_STATUS_OK
}

Status int_float(KodObject* self, f64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (f64)((KodObjectInt*)self)->_int;
    RETURN_STATUS_OK
}

Status int_bool(KodObject* self, bool* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (bool)((KodObjectInt*)self)->_int;
    RETURN_STATUS_OK
}

KodObjectNumberMethods int_as_number = {
    .add=int_add,
    .sub=int_sub,
    .mul=int_mul,
    .div=int_div,

    .gt=int_gt,

    ._int=int_int,
    ._float=int_float,
    ._bool=int_bool,
};

KodObjectType KodType_Int = {
    TYPE_HEADER("int")
    .as_number=&int_as_number,
    .as_subscript=0,
    .str=int_str,
    .hash=0,
    .call=0,
    .free=kod_object_free
};