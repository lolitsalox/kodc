#pragma once

#include "type.h"

typedef struct Tuple {
    KodObject** items;
    size_t size;
} Tuple;

typedef struct KodTupleObject {
    KodObject object;
    Tuple _tuple;
} KodTupleObject;

extern KodTypeObject KodType_Tuple;