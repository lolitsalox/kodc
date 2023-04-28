#include "kod_object_bool.h"
#include "kod_object_int.h"
#include "kod_object_float.h"

static inline i64 b_add(bool a, bool b) { return a + b; }
static inline i64 b_sub(bool a, bool b) { return a - b; }
static inline i64 b_mul(bool a, bool b) { return a * b; }

Status bool_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    
    char* buffer = _strdup(((KodObjectBool*)self)->_bool ? "true" : "false");
    if (!buffer) RETURN_STATUS_FAIL("strdup failed");

    *out = buffer;
    RETURN_STATUS_OK
}

static Status bool_bin(KodObject* self, KodObject* other, KodObject** out, i64 (*op)(i64 a, i64 b), bool return_int) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!self->type->as_number) RETURN_STATUS_FAIL("self Type has no number representation")
    if (!other->type->as_number) RETURN_STATUS_FAIL("other Type has no number representation")

    if (self->type->as_number->_bool && other->type->as_number->_bool) {
        i64 left = 0;
        Status s = self->type->as_number->_bool(self, (bool*) & left);
        if (s.type == ST_FAIL) return s;

        i64 right = 0;
        s = other->type->as_number->_bool(other, (bool*) & right);
        if (s.type == ST_FAIL) return s;
        
        if (return_int) {
            if ((s = kod_object_new_int(op(left, right), (KodObjectInt**)out)).type == ST_FAIL) return s;
        }
        else if ((s = kod_object_new_bool(op(left, right), (KodObjectBool**)out)).type == ST_FAIL) return s;
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Type has no int representation")
}

Status bool_add(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT || other->kind == OBJECT_INT) return other->type->as_number->add(self, other, out);
    return bool_bin(self, other, out, b_add, true);
}

Status bool_sub(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT || other->kind == OBJECT_INT) return other->type->as_number->sub(self, other, out);
    return bool_bin(self, other, out, b_sub, true);
}

Status bool_mul(KodObject* self, KodObject* other, KodObject** out) {
    if (other->kind == OBJECT_FLOAT || other->kind == OBJECT_INT) return other->type->as_number->mul(self, other, out);
    return bool_bin(self, other, out, b_mul, true);
}

Status bool_div(KodObject* self, KodObject* other, KodObject** out) {
    return KodType_Float.as_number->div(self, other, out);
}

Status bool_int(KodObject* self, i64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (i64)((KodObjectBool*)self)->_bool;
    RETURN_STATUS_OK
}

Status bool_float(KodObject* self, f64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (f64)((KodObjectBool*)self)->_bool;
    RETURN_STATUS_OK
}

Status bool_bool(KodObject* self, bool* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = ((KodObjectBool*)self)->_bool;
    RETURN_STATUS_OK
}

KodObjectNumberMethods bool_as_number = {
    .add=bool_add,
    .sub=bool_sub,
    .mul=bool_mul,
    .div=bool_div,

    

    ._int=bool_int,
    ._float=bool_float,
    ._bool=bool_bool,

};

KodObjectType KodType_Bool = {
    TYPE_HEADER("bool")
    .as_number=&bool_as_number,
    .as_subscript=0,
    .str=bool_str,
    .hash=0,
    .call=0,
    .free=kod_object_free
};

KodObjectBool KodObject_True = {
    .base = {.kind = OBJECT_BOOL,
    .ref_count = 1,
    .type = &KodType_Bool},
    ._bool = true
};

KodObjectBool KodObject_False = {
    .base = {.kind = OBJECT_BOOL,
    .ref_count = 1,
    .type = &KodType_Bool},
    ._bool = false
};

Status kod_object_new_bool(bool value, KodObjectBool** out) {
    *out = value ? &KodObject_True : &KodObject_False;
    RETURN_STATUS_OK
}