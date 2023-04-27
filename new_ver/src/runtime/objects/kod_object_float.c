#include "kod_object_float.h"

static inline f64 f64_add(f64 a, f64 b) { return a + b; }

static inline f64 f64_sub(f64 a, f64 b) { return a - b; }

static inline f64 f64_mul(f64 a, f64 b) { return a * b; }

static inline f64 f64_div(f64 a, f64 b) { return a / b; }

Status kod_object_new_float(f64 value, KodObjectFloat** out) {
    KodObjectFloat* obj = malloc(sizeof(KodObjectFloat));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for float object");

    obj->_float = value;
    obj->base.kind = OBJECT_FLOAT;
    obj->base.type = &KodType_Float;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status float_str_impl(KodObjectFloat* self, char** out) {
    char* buffer = malloc(32);
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer");
    _gcvt_s(buffer, 32, self->_float, 16);
    *out = buffer;
    RETURN_STATUS_OK
}

Status float_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return float_str_impl((KodObjectFloat*)self, out);
}

static Status float_bin(KodObject* self, KodObject* other, KodObject** out, f64 (*op)(f64 a, f64 b)) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (!self->type->as_number) RETURN_STATUS_FAIL("self Type has no number representation")
    if (!other->type->as_number) RETURN_STATUS_FAIL("other Type has no number representation")

    if (self->type->as_number->_float && other->type->as_number->_float) {
        f64 left = 0;
        Status s = self->type->as_number->_float(self, &left);
        if (s.type == ST_FAIL) return s;

        f64 right = 0;
        s = other->type->as_number->_float(other, &right);
        if (s.type == ST_FAIL) return s;

        if (op == f64_div && right == 0) RETURN_STATUS_FAIL("Division by zero")

        if ((s = kod_object_new_float(op(left, right), (KodObjectFloat**)out)).type == ST_FAIL) return s;
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Type has no float representation")
}

Status float_add(KodObject* self, KodObject* other, KodObject** out) {
    return float_bin(self, other, out, f64_add);
}

Status float_sub(KodObject* self, KodObject* other, KodObject** out) {
    return float_bin(self, other, out, f64_sub);
}

Status float_mul(KodObject* self, KodObject* other, KodObject** out) {
    return float_bin(self, other, out, f64_mul);
}

Status float_div(KodObject* self, KodObject* other, KodObject** out) {
    return float_bin(self, other, out, f64_div);
}

Status float_float(KodObject* self, f64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = ((KodObjectFloat*)self)->_float;
    RETURN_STATUS_OK
}

Status float_int(KodObject* self, i64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (i64)((KodObjectFloat*)self)->_float;
    RETURN_STATUS_OK
}

KodObjectNumberMethods float_as_number = {
    .add=float_add,
    .sub=float_sub,
    .mul=float_mul,
    .div=float_div,

    ._int=float_int,
    ._float=float_float,
    ._bool=0,

};

KodObjectType KodType_Float = {
    TYPE_HEADER("float")
    .as_number=&float_as_number,
    .as_subscript=0,
    .str=float_str,
    .hash=0,
    .call=0,
    .free=kod_object_free
};