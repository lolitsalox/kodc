#pragma once

#include "type.h"

#define INITIAL_TABLE_SIZE 1024
#define LOAD_FACTOR 0.75

typedef struct DictItem {
    KodObject* key;
    KodObject* value;
    struct DictItem* next;
} DictItem;

typedef struct Dict {
    DictItem** table;
    size_t size;  // size of table
    size_t count; // how many
} Dict;  

typedef struct KodDictObject {
    KodObject object;
    Dict _dict;
} KodDictObject;

extern KodTypeObject KodType_Dict;

enum DICT_METHODS {
    DICT_METHOD_GET,
    DICT_METHOD_SET,
};