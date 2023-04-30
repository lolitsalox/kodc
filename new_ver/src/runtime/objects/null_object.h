#pragma once

#include "type_object.h"

typedef struct KodObjectNull {
    KodObject base;
} KodObjectNull;

extern KodObjectNull KodObject_Null;

Status kod_object_new_null(KodObjectNull** out);