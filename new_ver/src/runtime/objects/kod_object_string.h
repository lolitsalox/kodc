#pragma once

#include "kod_object_type.h"

typedef struct KodObjectString {
    KodObject base;
    char* _string;
    size_t _len;
} KodObjectString;

extern KodObjectType KodType_String;

Status kod_object_new_string(char* str, KodObjectString** out);