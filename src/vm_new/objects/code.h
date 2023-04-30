#pragma once

#include "type.h"
#include "../../compiler/compiler.h"

typedef struct KodCodeObject {
    KodObject object;
    Code _code;
} KodCodeObject;

extern KodTypeObject KodType_Code;