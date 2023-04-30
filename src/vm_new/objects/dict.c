#include "dict.h"

#include "string.h"
#include "tuple.h"
#include "int.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

KodStringObject* dict_repr(KodDictObject* self) {

    char str[MAX_REPR_BUFFER_SIZE] = { 0 };
    snprintf(str, MAX_REPR_BUFFER_SIZE, "<dict object at %p>", self);
    return string_new_from_string(str);

    UNIMPLEMENTED;
    KodStringObject* result = string_new_from_string(NULL);
    for (size_t i = 0; i < self->_dict.size; ++i) {
        DictItem* dict_item = self->_dict.table[i];
        if (!dict_item) continue;
        
        KodType_String.as_number->kod_add(
            As_Object(result),
            dict_item->key->type_object->repr(dict_item->key, NULL, NULL)
        );

        KodType_String.as_number->kod_add(
            As_Object(result), 
            dict_item->value->type_object->repr(dict_item->value, NULL, NULL)
        );
    }

    return result;
}

Dict* dict_create() {
    Dict* dict = malloc(sizeof(Dict));
    dict->table = calloc(INITIAL_TABLE_SIZE, sizeof(DictItem*));
    dict->size = INITIAL_TABLE_SIZE;
    dict->count = 0;
    return dict;
}

void dict_resize(KodDictObject* self, size_t new_size) {
    self->_dict.size = new_size;
    self->_dict.table = realloc(self->_dict.table, new_size);
}

KodObject* dict_get(KodDictObject* self, KodObject* args, KodObject* kwargs) {
    if (!args->type_object->hash) {
        printf("TypeError: unhashable type: '%s'\n", args->type_object->type_name);
        UNIMPLEMENTED;
    }

    size_t index = args->type_object->hash(args) % self->_dict.size;
    DictItem* current = self->_dict.table[index];
    while (current != NULL) {
        if (current->key == args) {
            ref_object(args);
            return args;
        }
        current = current->next;
    }
    return null_new();
}

KodObject* dict_set(KodDictObject* self, KodObject* args, KodObject* kwargs) {
    if (!Is_Type(args, &KodType_Tuple)) {
        UNIMPLEMENTED;
    }
    
    KodTupleObject* tuple = (KodTupleObject*)args;
    if (tuple->_tuple.size != 2) {
        UNIMPLEMENTED;
    }

    KodObject *key = tuple->_tuple.items[0], 
            *value = tuple->_tuple.items[1];

    if (key->type_object->hash == NULL) {
        UNIMPLEMENTED;
    }

    // Calculate the hash of the key
    size_t hash = key->type_object->hash(key);

    // Get the bucket index for the hash
    size_t index = hash % self->_dict.size;

    // Find the DictItem with the matching key in the bucket
    DictItem* item = self->_dict.table[index];
    while (item != NULL) {
        if (item->key->type_object->eq == NULL) {
            UNIMPLEMENTED;
        }

        if ((KodIntObject*)item->key->type_object->eq(item->key, key, &Kod_Null)) {
            // Update the value of the DictItem
            deref_object(item->value);
            item->value = value;
            ref_object(value);
            return null_new();
        }
        item = item->next;
    }
    // If the key was not found, create a new DictItem and insert it at the beginning of the bucket
    DictItem* new_item = (DictItem*)malloc(sizeof(DictItem));
    new_item->key = key;
    new_item->value = value;
    new_item->next = NULL;
    self->_dict.table[index] = new_item;

    // Update the count of the dictionary
    ++self->_dict.count;

    // Check if the load factor has been exceeded, and resize the table if necessary
    if ((float)self->_dict.count / self->_dict.size > LOAD_FACTOR) {
        dict_resize(self, self->_dict.size * 2);
    }

    return null_new();
}


KodDictObject* dict_new(KodTypeObject* tp, KodObject* args, KodObject* kwargs) {
    if (tp != &KodType_Dict) {
        UNIMPLEMENTED;
    }
    
    if (Is_Null(args)) {
        // items and size are zeroed 
        KodDictObject* new_dict = calloc(1, sizeof(KodDictObject));
        ref_object(As_Object(new_dict));
        new_dict->object.type = OBJECT_DICT;
        new_dict->object.type_object = &KodType_Dict;
        new_dict->_dict.size = INITIAL_TABLE_SIZE;
        new_dict->_dict.table = calloc(INITIAL_TABLE_SIZE, sizeof(KodObject*));
        return new_dict;
    }

    UNIMPLEMENTED;
    KodDictObject* result = NULL;
    return result;
}

#define dict_doc \
"dict object"

Kod_MethodDef dict_methods[] = {
    {.name="__get__", .doc="get value by key", .flags=0, .method=(methfunc)dict_get},
    {.name="__set__", .doc="sets a value using key", .flags=0, .method=(methfunc)dict_set},
    {0},
};

KodTypeObject KodType_Dict = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="dict",
    .methods=dict_methods,
    .as_number=0,
    .hash=0,
    .call=0,
    .repr=(methfunc)dict_repr,
    .new=(typefunc)dict_new,
    .free=deref_object,
    .doc=dict_doc,
    .eq=0,
    .size=sizeof(char*),
};
