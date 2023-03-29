
#include "null.h"
#include "bool.h"
#include "string.h"
#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment bool_attributes;


static Kod_Object* native_bool_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    if (self->_bool) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, "true") == 0) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_string_object("true");
    }
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, "false") == 0) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_string_object("false");
}

static Kod_Object* native_bool_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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


void init_bool_attributes() {
    init_environment(&bool_attributes);
    ADD_METHOD(bool, str);
    ADD_METHOD(bool, bool);
}

Kod_Object* new_bool_object(bool value) {
    debug_print("creating bool object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &bool_attributes);
    Kod_Object* obj = new_object(OBJECT_BOOL, attributes);
    obj->_bool = value;
    debug_print("new bool object at %p - %d\n", obj, obj->_bool);
    return obj;
}

Environment* get_bool_attributes() {
    return &bool_attributes;
}