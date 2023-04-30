#pragma once

#include "type_object.h"

typedef struct KodObjectString {
    KodObject base;
    char* _string;
    size_t _len;
} KodObjectString;

extern KodObjectType KodType_String;

Status kod_object_new_string(char* str, KodObjectString** out);
Status kod_object_initialize_string();
Status kod_object_destroy_string();
