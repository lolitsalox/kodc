#pragma once

#include "type_object.h"

typedef struct KodObjectFloat {
    KodObject base;
    f64 _float;
} KodObjectFloat;

extern KodObjectType KodType_Float;

Status kod_object_new_float(f64 value, KodObjectFloat** out);