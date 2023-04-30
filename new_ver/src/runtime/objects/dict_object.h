 #pragma once

 #include "type_object.h"

 typedef struct KodObjectEntry {
     KodObject* key;
     KodObject* value;
     struct KodObjectEntry* next;
 } KodObjectEntry;

 typedef struct KodObjectDict {
     KodObject base;
     KodObjectEntry** table;
     size_t size;
     size_t capacity;
 } KodObjectDict;

 extern KodObjectType KodType_Dict;