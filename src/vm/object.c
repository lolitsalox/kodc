#include "object.h"

#include <stdlib.h>
#include <stdio.h>

static Environment null_attributes;
static Environment bool_attributes;
static Environment int_attributes;
static Environment string_attributes;
static Environment code_attributes;

static Environment native_functions;

Kod_Object* native_string_method_str(Kod_Object** args, size_t argc) {
    return new_string_object(args[0]->_string);
}

Kod_Object* native_null_method_bool(Kod_Object** args, size_t argc) {
    return new_int_object(0);// TODO: change to new bool
}

Kod_Object* native_null_method_str(Kod_Object** args, size_t argc) {
    return new_string_object("null");
}

Kod_Object* native_int_method_bool(Kod_Object** args, size_t argc) {
    if (!args || !args[0]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(args[0]->_int);// TODO: change to new bool
}

Kod_Object* native_int_method_unary_add(Kod_Object** args, size_t argc) {
    if (!args || !args[0]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(args[0]->_int);
}

Kod_Object* native_int_method_unary_sub(Kod_Object** args, size_t argc) {
    if (!args || !args[0]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(-args[0]->_int);
}

Kod_Object* native_int_method_binary_lt(Kod_Object** args, size_t argc) {
    if (argc < 2 || !args || !args[0] || !args[1]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(args[0]->_int < args[1]->_int);
}

Kod_Object* native_int_method_binary_add(Kod_Object** args, size_t argc) {
    if (argc < 2 || !args || !args[0] || !args[1]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(args[0]->_int + args[1]->_int);
}

Kod_Object* native_int_method_binary_sub(Kod_Object** args, size_t argc) {
    if (argc < 2 || !args || !args[0] || !args[1]) {
        printf("line: %d\n", __LINE__);
        return new_null_object();
    }
    return new_int_object(args[0]->_int - args[1]->_int);
}

Kod_Object* native_int_method_str(Kod_Object** args, size_t argc) {
    Kod_Object* self = args[0];
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
    Kod_Object* obj = new_string_object(str);
    return obj;
}

// Kod_Object* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, Environment* initial_env);
Kod_Object* native_print(Kod_Object** args, size_t argc) {
    if (!args) goto end;
    for (size_t i = 0; i < argc; ++i) {
        if (args[i]) {
            Kod_Object* str = get_environment(args[i]->attributes, "__str__");
            if (!str) {
                printf("<%s object at 0x%p>", object_type_to_str(args[i]->type), args[i]);
                continue;
            }
            switch (str->type) {
                case OBJECT_NATIVE_FUNCTION:
                    Kod_Object* str_args[] = {args[i]};
                    Kod_Object* str_object = str->_function.callable(str_args, 1);
                    printf("%s", str_object->_string);
                    deref_object(str_object);
                    break;
                case OBJECT_CODE:
                    // Kod_Object str_object = run_code_object(); // todo: later
                    fputs("Unimplemented", stderr);
                    break;
                default:
                    fputs("str is not callable??", stderr);
                    break;
            }
        }
    }
    end:
    puts("");
    return new_null_object();
}


void init_native_attributes() {
    init_environment(&null_attributes);
    set_environment(&null_attributes, 
        (ObjectNamePair){
            .name="__bool__", 
            new_native_function_object("__bool__", native_null_method_bool)
        }
    );
    set_environment(&null_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_null_method_str)
        }
    );

    init_environment(&int_attributes);
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__bool__", 
            new_native_function_object("__bool__", native_int_method_bool)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_int_method_str)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__unary_add__", 
            new_native_function_object("__unary_add__", native_int_method_unary_add)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__unary_sub__", 
            new_native_function_object("__unary_sub__", native_int_method_unary_sub)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__lt__", 
            new_native_function_object("__lt__", native_int_method_binary_lt)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__add__", 
            new_native_function_object("__add__", native_int_method_binary_add)
        }
    );
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__sub__", 
            new_native_function_object("__sub__", native_int_method_binary_sub)
        }
    );

    init_environment(&string_attributes);
    set_environment(&string_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_string_method_str)
        }
    );
}

void init_native_functions() {
    init_environment(&native_functions);
    set_environment(&native_functions, 
        (ObjectNamePair){
            .name="print", 
            new_native_function_object("print", native_print)
        }
    );
}

Environment* get_native_functions() {
    return &native_functions;
}

Kod_Object* new_object(enum Kod_ObjectType type, Environment* attributes) {
    Kod_Object* object = malloc(sizeof(Kod_Object));
    object->type = type;
    object->ref_count = 1;
    object->attributes = attributes;
    return object;
}

Kod_Object* new_null_object() {
    Environment* attributes = new_environment();
    update_environment(attributes, &null_attributes);
    return new_object(OBJECT_NULL, attributes);
}

Kod_Object* new_bool_object(bool value) {
    Environment* attributes = new_environment();
    update_environment(attributes, &bool_attributes);
    Kod_Object* object = new_object(OBJECT_BOOL, attributes);
    object->_bool = value;
    return object;
}

Kod_Object* new_int_object(int64_t value) {
    Environment* attributes = new_environment();
    update_environment(attributes, &int_attributes);
    Kod_Object* object = new_object(OBJECT_INTEGER, attributes);
    object->_int = value;
    return object;
}

Kod_Object* new_string_object(char* value) {
    Environment* attributes = new_environment();
    update_environment(attributes, &string_attributes);
    Kod_Object* object = new_object(OBJECT_STRING, attributes);
    size_t size = strlen(value) + 1;
    object->_string = calloc(size, sizeof(char));
    strncpy(object->_string, value, size);
    return object;
}

Kod_Object* new_code_object(Code value) {
    Environment* attributes = new_environment();
    update_environment(attributes, &code_attributes);
    Kod_Object* object = new_object(OBJECT_CODE, attributes);
    object->_code = value;
    return object;
}

Kod_Object* new_native_function_object(char* name, NativeFunction callable) {
    Kod_Object* object = new_object(OBJECT_NATIVE_FUNCTION, NULL);
    object->_function.name = name;
    object->_function.callable = callable;
    return object;
}

void free_object(Kod_Object* object) {
    if (!object) return;
    switch (object->type) {
        case OBJECT_STRING:
            if (object->_string)
                free(object->_string);
            break;
        case OBJECT_CODE:
            free_code(object->_code);
            break;
        default: break;
    }

    // free_environment(object->attributes);
    // free(object->attributes);
    // free(object);
}

void ref_object(Kod_Object* object) {
    if (!object) return;
    ++object->ref_count;
}

bool deref_object(Kod_Object* object) {
    if (!object) return false;
    if (object->ref_count-- <= 0) {
        free_object(object);
        return true;
    }
    return false;
}

String object_type_to_str(enum Kod_ObjectType type) {
    switch (type) {
        case OBJECT_NULL: return "NULL";
        case OBJECT_BOOL: return "BOOL"; 
        case OBJECT_INTEGER: return "INTEGER";
        case OBJECT_FLOAT: return "FLOAT";
        case OBJECT_STRING: return "OBJECT_STRING";
        case OBJECT_CODE: return "CODE";
        case OBJECT_NATIVE_FUNCTION: return "NATIVE_FUNCTION";
    }
    return "OBJECT_UNKNOWN";
}