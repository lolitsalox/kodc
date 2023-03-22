#include "object.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

const char* object_type_to_str(enum ObjectType type) {
    switch (type) {
        case OBJECT_INT:        return "INT";
        case OBJECT_FLOAT:      return "FLOAT";
        case OBJECT_STRING:     return "STRING";
        case OBJECT_CODE:       return "CODE";
        case OBJECT_NATIVE_FUNCTION:   return "NATIVE_FUNCTION";
        case OBJECT_TYPE:       return "TYPE";
        case OBJECT_NULL:       return "NULL";
    }
    return "UNKNOWN";
}

void print_object(Object* object, uint32_t indent_level) {
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

        case OBJECT_CODE: {
            // printf("<code object %.*s at %p>\n", object->_code.name, object);
            break;
        }

        case OBJECT_NATIVE_FUNCTION: {
            // printf("<native function %.*s>\n", object->_native_function.name.length, object->_native_function.name.value);
            break;
        }
        case OBJECT_TYPE: {
            Object* name = env_get_variable(object->attributes, "name");

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

Object* new_object(Object object) {
    Object* object_pointer = malloc(sizeof(Object));
    if (!object_pointer) {
        printf("[object]: Error - coudln't allocate for object\n");
        return NULL;
    }
    object.ref_count = 0;
    // object.from_return = false;
    *object_pointer = object;
    return object_pointer;
}

bool delete_object(Object* object) {
    if (!object) return false;
    // TODO: deep delete
    free_env(object->attributes);
    switch (object->object_type) {
        case OBJECT_STRING:
            free(object->_string);
        case OBJECT_CODE:
            break;

        default: break;
    }
    return true;
}

void free_object(Object* object) {
    if (delete_object(object)) free(object);
    // printf(">>> FREEING %s\n", object_type_to_str(object->object_type));
    // if (object->attributes && !object->attributes->is_global)
    //     env_free(object->attributes);
}

void ref_object(Object* object) {
    if (!object) return;
    object->ref_count++;
}

void deref_object(Object* object) {
    if (!object) return;
    object->ref_count--;
    if (object->ref_count <= 0) {
        free_object(object);
    }
}

ObjectNode init_object_node() {
    return (ObjectNode){
        .object=NULL,
        .down=NULL
    };
}

ObjectNode* new_object_node() {
    ObjectNode* object_node = malloc(sizeof(ObjectNode));
    *object_node = init_object_node();
    return object_node;
}

bool delete_object_node(ObjectNode* object_node) {
    if (!object_node) return false;
    free_object(object_node->object);
    return true;
}

void free_object_node(ObjectNode* object_node) {
    if (delete_object_node(object_node)) free(object_node);
}

ObjectStack init_object_stack() {
    return (ObjectStack){
        .top=NULL,
        .empty=true
    }
}

ObjectStack* new_object_stack() {
    ObjectStack* object_stack = malloc(sizeof(ObjectStack));
    *object_stack = init_object_stack();
    return object_stack;
}

void object_stack_push(ObjectStack* object_stack, Object object) {
    if (!object_stack) return;
    
    ObjectNode* object_node = new_object_node();
    if (!new_node) return;
    
    object_node->object = object; // TODO: deep copy
    object_node->down = object_stack->top;

    object_stack->top = object_node;
}

Object object_stack_top(ObjectStack* object_stack) {
    return object_stack->top.object; // TODO: deep copy
}

Object object_stack_pop(ObjectStack* object_stack) {
    Object object_to_return = object_stack_top(object_stack);
    
    free_object_node(object_stack->top);
    return object_to_return;
}


bool delete_object_stack(ObjectStack* object_stack) {
    ObjectNode* tmp;
    while ()
}

void free_object_stack(ObjectStack* object_stack) {
    if (!delete_object_stack(object_stack)) return;
    free(object_stack);
}