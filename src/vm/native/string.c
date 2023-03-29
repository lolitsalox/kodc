
#include "string.h"
#include "null.h"
#include "bool.h"
#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment string_attributes;

static Kod_Object* native_string_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 1) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object(); 
    }
    Kod_Object* self = args[0];
    ref_object(self);
    return self;
}

static Kod_Object* native_string_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 1) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object(); 
    }

    Kod_Object* self = args[0];
    if (self->_string[0] != '\0') {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_BOOL && vm->cop.data[i]._bool == true) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_bool_object(true);
    }
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_BOOL && vm->cop.data[i]._bool == false) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_bool_object(false);
}

void init_string_attributes() {
    init_environment(&string_attributes);
    ADD_METHOD(string, str);
    ADD_METHOD(string, bool);
}

Kod_Object* new_string_object(char* value) {
    debug_print("creating string object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &string_attributes);
    Kod_Object* obj = new_object(OBJECT_STRING, attributes);
    size_t size = strlen(value) + 1;
    obj->_string = calloc(size, sizeof(char));
    strncpy(obj->_string, value, size);
    debug_print("new string object at %p - %s\n", obj, obj->_string);
    return obj;
}

Environment* get_string_attributes() {
    return &string_attributes;
}