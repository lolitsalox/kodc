#include "object.h"

#include <stdio.h>
#include <stdlib.h>

const char* object_type_to_str(kod_object_type_t type) {
    switch (type) {
        case OBJECT_NUMBER:     return "NUMBER";
        case OBJECT_STRING:     return "STRING";
        case OBJECT_FUNCTION:   return "FUNCTION";
        case OBJECT_NATIVE_FUNCTION:   return "NATIVE_FUNCTION";
        case OBJECT_TYPE:       return "TYPE";
        case OBJECT_NONE:       return "NONE";
    }
    return "UNKNOWN";
}

void object_print(kod_object_t* object, uint32_t indent_level) {
    // If the object node is null, print a warning and return.
    if (!object) {
        printf("[object]: Warning - object is null\n");
        return;
    }

    // Print indentation.
    for (uint32_t i = 0; i < indent_level; ++i) printf("    ");
    // Print the type of the object node.
    printf("(%s): ", object_type_to_str(object->object_type));

    // Switch on the type of the object node.
    switch (object->object_type) {
        case OBJECT_NUMBER: {
            printf("%g\n", object->number);
            break;
        }
        case OBJECT_STRING: {
            printf("%.*s\n", object->string.length, object->string.value);
            break;
        }

        case OBJECT_FUNCTION: {
            printf("<function %.*s at %p>\n", object->function.function_node.name.length, object->function.function_node.name.value, object);
            break;
        }

        case OBJECT_NATIVE_FUNCTION: {
            printf("<native function %.*s>\n", object->native_function.name.length, object->native_function.name.value);
            break;
        }
        case OBJECT_TYPE: {
            kod_object_t* name = env_get_variable(object->type.attributes, (ast_string_t){.value="name", .length=sizeof("name") - 1});
            if (!name || name->object_type != OBJECT_STRING) {
                puts("[object]: Error - the type has no name or the name is not a string");
                exit(1);
            }
            printf("<type %.*s at %p>", name->string.length, name->string.value, object);
            break;
        }
        default:
            printf("TODO: implement object_print\n");
            break;
    }

}

kod_object_t* object_new(kod_object_t object) {
    kod_object_t* object_pointer = malloc(sizeof(kod_object_t));
    if (!object_pointer) {
        printf("[object]: Error - coudln't allocate for object\n");
        return NULL;
    }

    *object_pointer = object;
    return object_pointer;
}
