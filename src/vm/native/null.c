
#include "null.h"
#include "string.h"
#include "bool.h"
#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment null_attributes;

static Kod_Object* native_null_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_BOOL && vm->cop.data[i]._bool == false) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_bool_object(false);
}

static Kod_Object* native_null_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, "null") == 0) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_string_object("null");
}

void init_null_attributes() {
    init_environment(&null_attributes);
    ADD_METHOD(null, bool);
    ADD_METHOD(null, str);
}

Kod_Object* new_null_object() {
    debug_print("creating null object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &null_attributes);
    Kod_Object* obj = new_object(OBJECT_NULL, attributes);
    debug_print("new null object at %p\n", obj);
    return obj;
}

Environment* get_null_attributes() {
    return &null_attributes;
}