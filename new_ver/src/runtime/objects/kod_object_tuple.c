#include "kod_object_tuple.h"

Status kod_object_new_tuple(size_t size, KodObjectTuple** out) {
    KodObjectTuple* obj = malloc(sizeof(KodObjectTuple));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for tuple object")
    if (!out) RETURN_STATUS_FAIL("Invalid out")

    obj->data = calloc(size, sizeof(KodObject*));
    if (!obj->data) RETURN_STATUS_FAIL("Couldn't allocate for tuple data")
    obj->size = (i64)size;
    obj->base.kind = OBJECT_TUPLE;
    obj->base.type = &KodType_Tuple;
    obj->base.ref_count = 0;

    *out = obj;
    RETURN_STATUS_OK
}

static Status tuple_str_impl(KodObjectTuple* self, char** out) {
    Status s;
    char *buffer = NULL, *tmp = NULL, *template = NULL;

    for (i64 i = 0; i < self->size; i++) {
        template = "%s%s, ";
        tmp = NULL;
        if (!self->data[i] || !self->data[i]->type->str) continue;
        if (!buffer) {
            buffer = _strdup("(");
            if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate buffer")
        }
        if ((s = self->data[i]->type->str(self->data[i], &tmp)).type == ST_FAIL) return s;
        if (!tmp) RETURN_STATUS_FAIL("Couldn't get string from object")

        size_t size = strlen(buffer) + strlen(tmp) + 3; // "%s%s, "
        if (i == self->size - 1) {
            --size;
            template = "%s%s)";
        }
        void* p = realloc(buffer, size);
        if (!p) RETURN_STATUS_FAIL("Couldn't reallocate buffer")
        buffer = p;
        snprintf(buffer, size, template, buffer, tmp);
        if (tmp) free(tmp);
    }
    
    *out = buffer;
    RETURN_STATUS_OK
}

static Status tuple_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return tuple_str_impl((KodObjectTuple*)self, out);
}

static Status tuple_free(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    KodObjectTuple* tuple = (KodObjectTuple*)self;
    Status s;

    for (i64 i = 0; i < tuple->size; i++) {
        if ((s = kod_object_deref(tuple->data[i])).type == ST_FAIL) return s;
        tuple->data[i] = NULL;
    }

    if (tuple->data) {
        free(tuple->data);
        tuple->data = NULL;
    }
    tuple->size = 0;

    return kod_object_free(self);
}

KodObjectType KodType_Tuple = {
    TYPE_HEADER("tuple")
    .as_number=0,
    .as_subscript=0,
    .str=tuple_str,
    .hash=0,
    .call=0,
    .free=tuple_free
};