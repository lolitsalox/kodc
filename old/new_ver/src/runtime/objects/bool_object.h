#pragma once

#include "type_object.h"

typedef struct KodObjectBool {
    KodObject base;
    bool _bool;
} KodObjectBool;

extern KodObjectBool KodObject_True;
extern KodObjectBool KodObject_False;

extern KodObjectType KodType_Bool;

Status kod_object_new_bool(bool value, KodObjectBool** out);