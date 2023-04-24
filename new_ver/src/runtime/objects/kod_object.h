#pragma once

#include <defines.h>

typedef struct KodObject KodObject;

typedef Status (*method_func) (KodObject* self, KodObject* tuple, KodObject* kwargs, KodObject** out);
typedef Status (*binary_func) (KodObject* self, KodObject* other, KodObject** out);
typedef Status (*unary_func) (KodObject* self, KodObject** out);

typedef enum ObjectKind {
    OBJECT_INT,
    OBJECT_FLOAT,
    OBJECT_STRING,
    OBJECT_CODE,
    OBJECT_TYPE,
    OBJECT_CLASS,
    OBJECT_NULL,
    OBJECT_BOOL,
} ObjectKind;

struct KodObjectType;

struct KodObject {
    ObjectKind kind;
    i32 ref_count;
    struct KodObjectType* type;
};

Status kod_object_ref(KodObject* self);
Status kod_object_deref(KodObject* self);
Status kod_object_free(KodObject* self);