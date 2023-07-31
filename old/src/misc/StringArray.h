#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef char* String;

typedef struct StringArray {
    size_t size;
    String* items;
    bool is_heap_allocated;
} StringArray;

StringArray init_string_array();
StringArray* new_string_array();
void free_string_array(StringArray* string_array);

void append_string_array(StringArray* string_array, String string, size_t length);
size_t find_string_array(StringArray* string_array, String string, size_t length);