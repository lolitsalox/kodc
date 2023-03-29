#include "object.h"

#include <stdlib.h>
#include <stdio.h>

#include "vm.h"

int DEBUG = 0;

static Environment null_attributes;
static Environment bool_attributes;
static Environment int_attributes;
static Environment float_attributes;
static Environment string_attributes;
static Environment code_attributes;

static Environment native_functions;

Kod_Object* native_string_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_string_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_bool_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_bool_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_null_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_BOOL && vm->cop.data[i]._bool == false) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_bool_object(false);
}

Kod_Object* native_null_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_STRING && strcmp(vm->cop.data[i]._string, "null") == 0) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_string_object("null");
}

Kod_Object* native_int_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_float_method_bool(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_int_method_unary_add(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_int_method_unary_sub(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_int_method_binary_lt(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    return new_bool_object(self->_int < other->_int);
}

Kod_Object* native_int_method_binary_add(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    return new_int_object(self->_int + other->_int);
}

Kod_Object* native_int_method_binary_sub(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    return new_int_object(self->_int - other->_int);
}

Kod_Object* native_int_method_binary_mul(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    return new_int_object(self->_int * other->_int);
}


Kod_Object* native_float_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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

Kod_Object* native_int_method_str(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
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
    Kod_Object* obj = new_string_object(str);
    return obj;
}

extern Kod_Object* run_code_object(VirtualMachine* vm, Code* code, CallFrame* parent_call_frame, Environment* initial_env, CallFrame* saved_frame);

Kod_Object* native_print(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size) {
    if (!args) goto end;
    for (size_t i = 0; i < size; ++i) {
        if (args[i]) {
            Kod_Object* str = get_environment(&args[i]->attributes, "__str__");
            if (!str) {
                printf("<%s object at 0x%p>", object_type_to_str(args[i]->type), args[i]);
                continue;
            }
            switch (str->type) {
                case OBJECT_NATIVE_FUNCTION: {
                    Kod_Object* str_args[] = {args[i]};
                    Kod_Object* str_object = str->_function.callable(vm, parent_call_frame, str_args, 1);
                    printf("%s ", str_object->_string);
                    if (DEBUG) puts("");
                    deref_object(str_object);
                    break;
                }
                case OBJECT_CODE: {
                    Environment env;
                    init_environment(&env);
                    set_environment(&env, (ObjectNamePair){.name=str->_code.params.items[0], .object=args[i]});
                    Kod_Object* str_object = run_code_object(vm, &str->_code, parent_call_frame, &env, NULL);
                    if (str_object->type == OBJECT_STRING)
                        printf("%s ", str_object->_string);
                    else
                        puts("RuntimeError: did not recieve a string object from __str__");
                    if (DEBUG) puts("");
                    deref_object(str_object);
                    break;
                }
                default:
                    fputs("str is not callable??", stderr);
                    break;
            }
        }
    }
    end:
    puts("");
    for (size_t i = 0; i < vm->cop.size; ++i) {
        if (vm->cop.data[i].type == OBJECT_NULL) {
            ref_object(&vm->cop.data[i]);
            return &vm->cop.data[i];
        }
    }
    return new_null_object(); 
}

#include <time.h>

Kod_Object* native_time(VirtualMachine* vm, CallFrame* parent_call_frame, Kod_Object** args, size_t size)  {
    return new_int_object(time(NULL));
}

void free_native_attributes() {
    if (DEBUG) puts("\nFREEING NULL ATTRIBUTES");
    free_environment(&null_attributes);
    if (DEBUG) puts("\nFREEING BOOL ATTRIBUTES");
    free_environment(&bool_attributes);
    if (DEBUG) puts("\nFREEING INT ATTRIBUTES");
    free_environment(&int_attributes);
    if (DEBUG) puts("\nFREEING STRING ATTRIBUTES");
    free_environment(&string_attributes);
    if (DEBUG) puts("\nFREEING CODE ATTRIBUTES");
    free_environment(&code_attributes);
}

void free_native_functions() {
    debug_print("FREEING NATIVE FUNCTIONS\n%s", "");
    free_environment(&native_functions);
}

void free_native() {
    free_native_functions();
    free_native_attributes();
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
    set_environment(&int_attributes, 
        (ObjectNamePair){
            .name="__mul__", 
            new_native_function_object("__mul__", native_int_method_binary_mul)
        }
    );

    
    init_environment(&float_attributes);
    set_environment(&float_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_float_method_str)
        }
    );
    set_environment(&float_attributes, 
        (ObjectNamePair){
            .name="__bool__", 
            new_native_function_object("__bool__", native_float_method_bool)
        }
    );

    init_environment(&string_attributes);
    set_environment(&string_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_string_method_str)
        }
    );
    set_environment(&string_attributes, 
        (ObjectNamePair){
            .name="__bool__", 
            new_native_function_object("__bool__", native_string_method_bool)
        }
    );

    init_environment(&bool_attributes);
    set_environment(&bool_attributes, 
        (ObjectNamePair){
            .name="__str__", 
            new_native_function_object("__str__", native_bool_method_str)
        }
    );
    set_environment(&bool_attributes, 
        (ObjectNamePair){
            .name="__bool__", 
            new_native_function_object("__bool__", native_bool_method_bool)
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
    set_environment(&native_functions, 
        (ObjectNamePair){
            .name="time", 
            new_native_function_object("time", native_time)
        }
    );
}

Environment* get_native_functions() {
    return &native_functions;
}

Kod_Object* new_object(enum Kod_ObjectType type, Environment attributes) {
    Kod_Object* object = malloc(sizeof(Kod_Object));
    object->type = type;
    object->ref_count = 1;
    object->attributes = attributes;
    return object;
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

Kod_Object* new_code_object(Code value) {
    debug_print("creating code object\n%s", "");
    Environment attributes;
    init_environment(&attributes);
    update_environment(&attributes, &code_attributes);
    Kod_Object* obj = new_object(OBJECT_CODE, attributes);
    obj->_code.parent_closure = NULL;
    // obj->_code = deep_copy_code(value);
    debug_print("new code object at %p - ", obj);
    if (DEBUG) print_code(&obj->_code, "\n");
    return obj;
}

Kod_Object* new_native_function_object(char* name, NativeFunction callable) {
    Kod_Object* object = new_object(OBJECT_NATIVE_FUNCTION, (Environment){0});
    object->_function.name = name;
    object->_function.callable = callable;
    object->ref_count = 1;
    debug_print("new native function object at %p - %s\n", object, name);
    return object;
}

void free_object(Kod_Object* object) {
    if (!object) return;
    debug_print("freeing object from type %s - ", object_type_to_str(object->type));
    switch (object->type) {
        case OBJECT_NULL: if (DEBUG) puts(""); break;
        case OBJECT_BOOL: if (DEBUG) printf("%d\n", object->_bool); break;
        case OBJECT_INTEGER: if (DEBUG) printf("%lld\n", object->_int); break;
        // case OBJECT_FLOAT:

        case OBJECT_STRING:
            if (object->_string) {
                if (DEBUG) printf("\"%s\"\n", object->_string);
                free(object->_string);
            }
            break;
        case OBJECT_CODE:
            if (DEBUG) print_code(&object->_code, "\n");
            free_code(object->_code);
            break;
        case OBJECT_NATIVE_FUNCTION: if (DEBUG) puts(object->_function.name); break; 
        default: break;
    }

    if (object->type != OBJECT_NATIVE_FUNCTION) free_environment(&object->attributes);
    free(object);
}

void ref_object(Kod_Object* object) {
    if (!object) return;
    ++object->ref_count;
    debug_print("ref object %s at %p -> %d\n", object_type_to_str(object->type), object, object->ref_count);
}

bool deref_object(Kod_Object* object) {
    if (!object) return false;
    --object->ref_count;
    debug_print("deref object %s at %p -> %d\n", object_type_to_str(object->type), object, object->ref_count);
    if (object->ref_count <= 0) {
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

Environment get_null_attributes() {
    return null_attributes;
}

Environment get_bool_attributes() {
    return bool_attributes;
}

Environment get_int_attributes() {
    return int_attributes;
}

Environment get_float_attributes() {
    return float_attributes;
}

Environment get_string_attributes() {
    return string_attributes;
}

Environment get_code_attributes() {
    return code_attributes;
}