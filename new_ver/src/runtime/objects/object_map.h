#pragma once

#include "kod_object.h"

#define MAX_TABLE_SIZE 32

typedef struct ObjectEntry {
    char* key;
    KodObject* value;
    struct ObjectEntry* next;
} ObjectEntry;

typedef struct ObjectMap {
    ObjectEntry* table[MAX_TABLE_SIZE];
    size_t size;
    size_t capacity;
} ObjectMap;

Status object_map_init(ObjectMap* map);
Status object_map_insert(ObjectMap* map, char* key, KodObject* value);
Status object_map_find(ObjectMap* map, char* key, KodObject** out); // returns null if didnt find
Status object_map_get(ObjectMap* map, char* key, KodObject** out);  // returns status failed if didnt find
Status object_map_remove(ObjectMap* map, char* key);
Status object_map_clear(ObjectMap* map);

Status object_map_ref(ObjectMap* map);
Status object_map_deref(ObjectMap* map);

void object_map_print(ObjectMap* map);