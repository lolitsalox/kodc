#include "object.h"

#include <stdlib.h>
#include <stdio.h>

#include "../../compiler/compiler.h"

#include "int.h"
#include "string.h"
#include "code.h"

int DEBUG = 0;

void free_object(KodObject* object) {
    if (!object) return;
    debug_print("freeing object from type %s - ", object_type_to_str(object->type));
    switch (object->type) {
        case OBJECT_NULL: if (DEBUG) puts(""); break;
        // case OBJECT_BOOL: if (DEBUG) printf("(%d)\n", ((KodBoolObject*)object)->_bool); break;
        case OBJECT_INTEGER: if (DEBUG) printf("(%lld)\n", ((KodIntObject*)object)->_int); break;
        // case OBJECT_FLOAT:

        case OBJECT_STRING:
            if (((KodStringObject*)object)->_string) {
                if (DEBUG) printf("\"%s\"\n", ((KodStringObject*)object)->_string);
                free(((KodStringObject*)object)->_string);
            }
            break;
        case OBJECT_CODE:
            if (DEBUG) print_code(&((KodCodeObject*)object)->_code, "\n", NULL, NULL);
            free_code(((KodCodeObject*)object)->_code);
            break;
        // case OBJECT_NATIVE_FUNCTION: if (DEBUG) puts((object)->_function.name); break; 
        default: break;
    }

    // if (object->type != OBJECT_NATIVE_FUNCTION) free_environment(&object->attributes);
    free(object);
}

KodObject* ref_object(KodObject* object) {
    if (!object) return NULL;
    ++object->ref_count;
    debug_print("ref object %s at %p -> %d\n", object_type_to_str(object->type), object, object->ref_count);
    return object;
}

bool deref_object(KodObject* object) {
    if (!object) return false;
    --object->ref_count;
    debug_print("deref object %s at %p -> %d\n", object_type_to_str(object->type), object, object->ref_count);
    if (object->ref_count <= 0) {
        free_object(object);
        return true;
    }
    return false;
}

char* object_type_to_str(enum KodObjectType type) {
    switch (type) {
        case OBJECT_NULL: return "NULL";
        case OBJECT_BOOL: return "BOOL"; 
        case OBJECT_INTEGER: return "INTEGER";
        case OBJECT_FLOAT: return "FLOAT";
        case OBJECT_STRING: return "OBJECT_STRING";
        case OBJECT_CODE: return "CODE";
        case OBJECT_NATIVE_FUNCTION: return "NATIVE_FUNCTION";
        case OBJECT_TYPE: return "TYPE";
        case OBJECT_OBJECT: return "OBJECT";
        case OBJECT_TUPLE: return "TUPLE";
        case OBJECT_DICT: return "DICT";
    }
    return "OBJECT_UNKNOWN";
}
