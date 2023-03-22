#include "StringArray.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringArray init_string_array() {
    return (StringArray){0};
}

StringArray* new_string_array() {
    StringArray* string_array = malloc(sizeof(StringArray));
    if (!string_array) return NULL;

    *string_array = init_string_array();
    string_array->is_heap_allocated = true;
    return string_array;
}

void free_string_array(StringArray* string_array) {
    if (!string_array) return;

    for (size_t i = 0; i < string_array->size; ++i) {
        free(string_array->items[i]);
    }

    if (string_array->is_heap_allocated)
        free(string_array);
}

/**
 * @brief Appends a string to the end of a string array
 * 
 * @param string_array string array object
 * @param string a string to be appended
 */
void append_string_array(StringArray* string_array, String string, size_t length) {
    // Check if string_array is NULL
    if (!string_array) {
        return;
    }

    String new_string = malloc((length + 1) * sizeof(char));
    if (!new_string) {
        fputs("[append_string_array] - Error: failed to allocate for new string.", stderr);
        return;
    }

    // Reallocate memory for the items array to make space for the new string
    String* tmp = realloc(string_array->items, (string_array->size + 1) * sizeof(String));
    if (!tmp) {
        free(new_string);
        fputs("[append_string_array] - Error: failed to reallocate for string array items.", stderr);
        return;
    }

    string_array->items = tmp;

    strncpy(new_string, string, length);
    new_string[length] = 0;
    // Add the new string to the end of the string array
    string_array->items[string_array->size++] = new_string;
}