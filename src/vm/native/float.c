
#include "float.h"
#include "null.h"
#include "bool.h"
#include "string.h"
#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment float_attributes;


static Kod_Object* native_float_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    if (self->_float) {
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



static Kod_Object* native_float_method_add(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object();
    }
    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    if (!self || !other) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object();
    }
    return new_float_object(self->_float + other->_float);
}



static Kod_Object* native_float_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    Kod_Object* self = args[0];
    if (!self) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object();
    }
    char str[64] = { 0 };
    gcvt(self->_float, 64, str);

    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, str) == 0) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    Kod_Object* obj = new_string_object(str);
    return obj;
}

void init_float_attributes() {
    init_environment(&float_attributes);
    ADD_METHOD(float, str);
    ADD_METHOD(float, bool);
    ADD_METHOD(float, add);
}

Kod_Object* new_float_object(double value) {
    debug_print("creating float object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &float_attributes);
    Kod_Object* obj = new_object(OBJECT_FLOAT, attributes);
    obj->_float = value;
    debug_print("new float object at %p - (%f)\n", obj, obj->_float);
    return obj;
}

Environment* get_float_attributes() {
    return &float_attributes;
}