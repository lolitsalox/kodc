#include "kod_object_string.h"
#include "kod_object_int.h"
#include "kod_object_bool.h"

Status kod_object_new_string(char* value, KodObjectString** out) {
    KodObjectString* obj = malloc(sizeof(KodObjectString));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for string object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    obj->_string = _strdup(value);
    if (!obj->_string) RETURN_STATUS_FAIL("strdup failed");
    obj->_len = strlen(obj->_string);
    obj->base.kind = OBJECT_STRING;
    obj->base.type = &KodType_String;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

Status str_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = _strdup(((KodObjectString*)self)->_string);
    if (!*out) RETURN_STATUS_FAIL("strdup failed");

    RETURN_STATUS_OK
}

Status str_add(KodObject* self, KodObject* other, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (other->type != &KodType_String) RETURN_STATUS_FAIL("other's type is not str");
 
    KodObjectString* s = (KodObjectString*)self, *o = (KodObjectString*)other;

    char* buffer = malloc(s->_len + o->_len + 1);
    if (!buffer) RETURN_STATUS_FAIL("Coudln't allocate for buffer");
        
    snprintf(buffer, s->_len + o->_len + 1, "%s%s", s->_string, o->_string);

    return kod_object_new_string(buffer, (KodObjectString*)out);
}

Status str_mul(KodObject* self, KodObject* other, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (other->type != &KodType_Int && other->type != &KodType_Bool) RETURN_STATUS_FAIL("Can't multiple string by a non-int");
    
    KodObjectString* so = (KodObjectString*)self;
    i64 mul = 0;
    Status s = other->type->as_number->_int(other, &mul);
    if (s.type == ST_FAIL) return s;
    if (mul < 0) RETURN_STATUS_FAIL("Can't multiple string by a negative value");

    char* buffer = calloc(1, mul * so->_len + 1);
    if (!buffer) RETURN_STATUS_FAIL("Failed to allocate for string");

    for (size_t i = 0; i < mul; ++i) {
        memcpy(buffer + i * so->_len, so->_string, so->_len + 1);
    }

    return kod_object_new_string(buffer, (KodObjectString*)out);
}

Status string_int(KodObject* self, i64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    
    *out = _strtoi64(((KodObjectString*)self)->_string, NULL, 10);
    RETURN_STATUS_OK
}

Status string_float(KodObject* self, f64* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = strtod(((KodObjectString*)self)->_string, NULL);
    RETURN_STATUS_OK
}

Status string_bool(KodObject* self, bool* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (bool)((KodObjectString*)self)->_len;
    RETURN_STATUS_OK
}

static Status str_free(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    KodObjectString* so = (KodObjectString*)self;

    if (so->_len) {
        free(so->_string);
        so->_string = "freed";
    }

    so->_len = 0;

    return kod_object_free(self);
}

KodObjectNumberMethods string_as_number = {
    .add = str_add,
    .mul = str_mul,

};

KodObjectType KodType_String = {
    TYPE_HEADER("str")
    .as_number = &string_as_number,
    .as_subscript = 0,
    .str = str_str,
    .hash = 0,
    .call = 0,
    .free = str_free
};