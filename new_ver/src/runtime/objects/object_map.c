#include "object_map.h"

#include "kod_object_type.h"

Status object_map_init(ObjectMap* map) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    for (int i = 0; i < MAX_TABLE_SIZE; i++) {
        map->table[i] = NULL;
    }
    map->size = 0;
    map->capacity = MAX_TABLE_SIZE;
    RETURN_STATUS_OK
}

Status object_map_insert(ObjectMap* map, char* key, KodObject* value) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    u64 index = string_hash(key) % MAX_TABLE_SIZE;

    ObjectEntry* entry = (ObjectEntry*)malloc(sizeof(ObjectEntry));
    if (!entry) {
        RETURN_STATUS_FAIL("malloc failed")
    }
    entry->key = _strdup(key);
    if (!entry->key) {
        RETURN_STATUS_FAIL("strdup failed")
    }

    if (map->table[index]) {
        if (strcmp(key, map->table[index]->key) == 0) {
            Status s;
            if ((s = object_map_remove(map, key)).type == ST_FAIL) return s;
        }
    }

#ifdef DEBUG_VM
    LOG("(+) Inserting object into map\n");
#endif
    entry->value = value;
    entry->next = map->table[index];
    map->table[index] = entry;
    map->size++;
    RETURN_STATUS_OK
}

Status object_map_find(ObjectMap* map, char* key, KodObject** out) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    if (!out) RETURN_STATUS_FAIL("out is null")
    u64 index = string_hash(key) % MAX_TABLE_SIZE;
    ObjectEntry* entry = map->table[index];
    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            *out = entry->value;
            RETURN_STATUS_OK
        }
        entry = entry->next;
    }
    *out = NULL;
    RETURN_STATUS_OK
}

Status object_map_get(ObjectMap* map, char* key, KodObject** out) {
    Status s = object_map_find(map, key, out);
    if (s.type == ST_FAIL) return s;
    if (!*out) RETURN_STATUS_FAIL("Key not in map")
    RETURN_STATUS_OK
}

Status object_map_remove(ObjectMap* map, char* key) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    u64 index = string_hash(key) % MAX_TABLE_SIZE;
    ObjectEntry* prev = NULL;
    ObjectEntry* curr = map->table[index];

    while (curr != NULL) {
        if (strcmp(curr->key, key) == 0) {
            if (prev == NULL) {
                map->table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
#ifdef DEBUG_VM
            LOG("(-) Removing object from map V\n");
#endif
            Status s = kod_object_deref(curr->value);
            if (s.type == ST_FAIL) return s;
            free(curr->key);
            free(curr);
            --map->size;
            // if there is no next
            if (curr == map->table[index]) {
                map->table[index] = NULL;
            }
            RETURN_STATUS_OK;
        }
        prev = curr;
        curr = curr->next;
    }
    RETURN_STATUS_FAIL("key not found")
}

Status object_map_clear(ObjectMap* map) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    for (int i = 0; i < MAX_TABLE_SIZE; i++) {
        ObjectEntry* entry = map->table[i];
        while (entry != NULL) {
            ObjectEntry* temp = entry;
            entry = entry->next;
            Status s = kod_object_deref(temp->value);
            if (s.type == ST_FAIL) return s;
            free(temp->key);
            free(temp);
        }
        map->table[i] = NULL;
    }
    map->size = 0;
    RETURN_STATUS_OK
}

void object_map_print(ObjectMap* map) {
    if (map == NULL) {
        printf("ObjectMap is NULL\n");
        return;
    }

    printf("ObjectMap: size=%zu, capacity=%zu\n", map->size, map->capacity);
    for (size_t i = 0; i < MAX_TABLE_SIZE; i++) {
        ObjectEntry* entry = map->table[i];
        while (entry != NULL) {
            printf("Index %zu: Key=%s Value=%p", i, entry->key, (void*)entry->value);
            if (entry->value->type->str) {
                char* buffer = NULL;
                Status s = entry->value->type->str(entry->value, &buffer);
                if (s.type == ST_FAIL) {
                    ERROR("KodRuntime", s.what);
                    free(s.what);
                    return;
                }

                printf("\t('%s' - %s)", entry->value->type->tp_name, buffer);
                free(buffer);
            }
            puts("");
            entry = entry->next;
        }
    }
}

Status object_map_ref(ObjectMap* map) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    UNIMPLEMENTED
    RETURN_STATUS_FAIL("unimplemented")
}

Status object_map_deref(ObjectMap* map) {
    if (!map) RETURN_STATUS_FAIL("map is null")
    UNIMPLEMENTED
    RETURN_STATUS_FAIL("unimplemented")
}
