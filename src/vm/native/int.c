
#include "int.h"
#include "null.h"
#include "bool.h"
#include "string.h"
#include "float.h"

#include "../vm.h"

#include <stdio.h>
#include <stdlib.h>

static Environment int_attributes;

static Kod_Object* native_int_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    if (self->_int) {
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

static Kod_Object* native_int_method_unary_bool_not(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 1) {
        return get_null_object(&vm->cop);
    }
    Kod_Object* self = args[0];
    return new_bool_object(!self->_int);
}

static Kod_Object* native_int_method_unary_add(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

static Kod_Object* native_int_method_unary_sub(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    if (!self) {
        for (size_t i = 0; i < vm->cop.size; ++i) {
            if (vm->cop.data[i].type == OBJECT_NULL) {
                ref_object(&vm->cop.data[i]);
                return &vm->cop.data[i];
            }
        }
        return new_null_object(); 
    }
    return new_int_object(-self->_int);
}

static Kod_Object* native_int_method_lt(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2 || !args[0] || !args[1]) {
        return get_null_object(&vm->cop);
    }

    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    Kod_Object* object = NULL;

    switch (other->type) {
        case OBJECT_INTEGER:    object = new_bool_object(self->_int < other->_int); break;
        case OBJECT_FLOAT:      object = new_bool_object(self->_int < other->_float); break;
        case OBJECT_BOOL:       object = new_bool_object(self->_int < other->_bool); break;
        default: return get_null_object(&vm->cop);
    }

    return object;
}

static Kod_Object* native_int_method_eq(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2 || !args[0] || !args[1]) {
        return get_null_object(&vm->cop);
    }

    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    Kod_Object* object = NULL;

    switch (other->type) {
        case OBJECT_INTEGER:    object = new_bool_object(self->_int == other->_int); break;
        case OBJECT_FLOAT:      object = new_bool_object(self->_int == other->_float); break;
        case OBJECT_BOOL:       object = new_bool_object(self->_int == other->_bool); break;
        default: return get_null_object(&vm->cop);
    }

    return object;
}

static Kod_Object* native_int_method_add(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2 || !args[0] || !args[1]) {
        return get_null_object(&vm->cop);
    }

    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    Kod_Object* object = NULL;

    switch (other->type) {
        case OBJECT_INTEGER:    object = new_int_object(self->_int + other->_int); break;
        case OBJECT_FLOAT:      object = new_float_object((double)self->_int + other->_float); break;
        case OBJECT_BOOL:       object = new_int_object(self->_int + (int64_t)other->_bool); break;
        default: return get_null_object(&vm->cop);
    }

    return object;
}

static Kod_Object* native_int_method_sub(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2 || !args[0] || !args[1]) {
        return get_null_object(&vm->cop);
    }

    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    Kod_Object* object = NULL;

    switch (other->type) {
        case OBJECT_INTEGER:    object = new_int_object(self->_int - other->_int); break;
        case OBJECT_FLOAT:      object = new_float_object((double)self->_int - other->_float); break;
        case OBJECT_BOOL:       object = new_int_object(self->_int - (int64_t)other->_bool); break;
        default: return get_null_object(&vm->cop);
    }

    return object;
}

static Kod_Object* native_int_method_mul(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (size < 2 || !args[0] || !args[1]) {
        return get_null_object(&vm->cop);
    }

    Kod_Object* self = args[0];
    Kod_Object* other = args[1];
    Kod_Object* object = NULL;

    switch (other->type) {
        case OBJECT_INTEGER:    object = new_int_object(self->_int * other->_int); break;
        case OBJECT_FLOAT:      object = new_float_object((double)self->_int * other->_float); break;
        case OBJECT_BOOL:       object = new_int_object(self->_int * (int64_t)other->_bool); break;
        default: return get_null_object(&vm->cop);
    }

    return object;
}

static Kod_Object* native_int_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    Kod_Object* self = args[0];
    if (!self) {
        return get_null_object(&vm->cop);
    }
    char str[64] = { 0 };
    size_t digits = 0;
    int64_t value = self->_int;
    bool neg = value < 0;
    size_t index = 0;
    if (value == 0) {
        str[0] = '0';
    }
    if (neg) {
        value = -value;
        str[index++] = '-';
        ++digits;
    }
    while (value) {
        ++digits;
        value /= 10;
    }
    value = self->_int * (neg ? -1 : 1);
    while (value) {
        str[digits - index++ - 1 + (neg ? 1 : 0)] = '0' + (value % 10);
        value /= 10;
    }

    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, str) == 0) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_string_object(str);
}

void init_int_attributes() {
    init_environment(&int_attributes);
    ADD_METHOD(int, bool);
    ADD_METHOD(int, str);
    ADD_METHOD(int, unary_bool_not);
    ADD_METHOD(int, unary_add);
    ADD_METHOD(int, unary_sub);
    ADD_METHOD(int, lt);
    ADD_METHOD(int, eq);
    ADD_METHOD(int, add);
    ADD_METHOD(int, sub);
    ADD_METHOD(int, mul);
}

Kod_Object* new_int_object(int64_t value) {
    debug_print("creating int object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &int_attributes);
    Kod_Object* obj = new_object(OBJECT_INTEGER, attributes);
    obj->_int = value;
    debug_print("new int object at %p - (%lld)\n", obj, obj->_int);
    return obj;
}

Environment* get_int_attributes() {
    return &int_attributes;
}