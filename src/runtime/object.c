#include "object.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const char* object_type_to_str(kod_object_type_t type) {
    switch (type) {
        case OBJECT_INT:        return "INT";
        case OBJECT_FLOAT:      return "FLOAT";
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
        case OBJECT_INT: {
            printf("%lld\n", object->_int);
            break;
        }
        case OBJECT_FLOAT: {
            printf("%f\n", object->_float);
            break;
        }
        case OBJECT_STRING: {
            printf("%s\n", object->_string);
            break;
        }

        case OBJECT_FUNCTION: {
            printf("<function %.*s at %p>\n", object->_function.function_node.name.length, object->_function.function_node.name.value, object);
            break;
        }

        case OBJECT_NATIVE_FUNCTION: {
            printf("<native function %.*s>\n", object->_native_function.name.length, object->_native_function.name.value);
            break;
        }
        case OBJECT_TYPE: {
            kod_object_t* name = env_get_variable(object->attributes, "name");

            if (!name || name->object_type != OBJECT_STRING) {
                puts("[object]: Error - name is not a string or name is null");
                exit(1);
            }
            printf("<type %s at %p>", name->_string, object);
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
    object.ref_count = 0;
    object.from_return = false;
    *object_pointer = object;
    return object_pointer;
}

void object_free(kod_object_t* object) {
    if (!object) {
        return;
    }

    switch (object->object_type) {
        case OBJECT_STRING:
            free(object->_string);
            break;

        default: break;
    }

    // printf(">>> FREEING %s\n", object_type_to_str(object->object_type));
    if (object->attributes && !object->attributes->is_global)
        env_free(object->attributes);

    free(object);
}

void object_inc_ref(kod_object_t* object) {
    if (!object) return;
    object->ref_count++;
}

void object_dec_ref(kod_object_t* object) {
    if (!object) return;
    object->ref_count--;
    if (object->ref_count <= 0) {
        object_free(object);
    }
}
