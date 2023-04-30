#pragma once

#include "type_object.h"

typedef struct KodObjectInt {
    KodObject base;
    i64 _int;
} KodObjectInt;

extern KodObjectType KodType_Int;

Status kod_object_new_int(i64 value, KodObjectInt** out);