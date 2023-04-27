#pragma once

#include "kod_object_type.h"

typedef struct KodObjectTuple {
    KodObject base;
    KodObject** data;
    i64 size;
} KodObjectTuple;

extern KodObjectType KodType_Tuple;

Status kod_object_new_tuple(size_t size, KodObjectTuple** out);