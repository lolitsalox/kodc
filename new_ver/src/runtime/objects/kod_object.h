#pragma once

#include <defines.h>

#define AS_OBJECT(obj) ((KodObject*)obj)

typedef struct KodObject KodObject;

typedef Status (*full_func)     (KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out);
typedef full_func method_func;
typedef Status (*normal_func)   (KodObject* args, KodObject* kwargs, KodObject** out);
typedef Status (*binary_func)   (KodObject* self, KodObject* other, KodObject** out);
typedef Status (*unary_func)    (KodObject* self, KodObject** out);
typedef Status (*kod_func)      (KodObject* self);

typedef enum ObjectKind {
    OBJECT_INT,
    OBJECT_FLOAT,
    OBJECT_STRING,
    OBJECT_FUNC,
    OBJECT_NATIVE_FUNC,
    OBJECT_TYPE,
    OBJECT_CLASS,
    OBJECT_NULL,
    OBJECT_BOOL,
    OBJECT_TUPLE,
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

u64 string_hash(char* key);