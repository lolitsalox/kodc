// #pragma once

// #include "kod_object_type.h"

// typedef struct KodObjectEntry {
//     KodObject* key;
//     KodObject* value;
//     struct KodObjectEntry* next;
// } KodObjectEntry;

// typedef struct ObjectDict {
//     KodObjectEntry** table;
//     size_t size;
//     size_t count;
// } ObjectDict;

// typedef struct KodObjectDict {
//     KodObject base;
//     ObjectDict _dict;
// } KodObjectDict;

// extern KodObjectType KodType_Dict;