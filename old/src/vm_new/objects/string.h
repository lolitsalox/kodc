#pragma once

#include "type.h"

typedef struct KodStringObject {
    KodObject object;
    char* _string;
} KodStringObject;

extern KodTypeObject KodType_String;

KodStringObject* string_new_from_string(char* str);

#define Is_String(a) Is_Type(a, &KodType_String)
#define As_String(a) ((KodStringObject*)a)
#define String_Value(a) As_String(a)->_string