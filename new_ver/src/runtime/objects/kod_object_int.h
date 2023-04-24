#pragma once

#include "kod_object_type.h"

typedef struct KodObjectInt {
    KodObject base;
    i64 _int;
} KodObjectInt;

extern KodObjectType KodObjectType_Int;