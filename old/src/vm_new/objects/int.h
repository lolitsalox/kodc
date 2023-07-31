#pragma once

#include "type.h"

typedef struct KodIntObject {
    KodObject object;
    int64_t _int;
} KodIntObject;

extern KodTypeObject KodType_Int;

#define As_Int(a) ((KodIntObject*)a)
#define Int_Value(a) As_Int(a)->_int