#include "object.h"

#include <stdlib.h>
#include <stdio.h>

#include "vm.h"
#include "native/null.h"
#include "native/bool.h"
#include "native/int.h"
#include "native/float.h"
#include "native/string.h"
#include "native/code.h"

int DEBUG = 0;

static Environment native_functions;
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
    free_environment(get_null_attributes());

    if (DEBUG) puts("\nFREEING BOOL ATTRIBUTES");
    free_environment(get_bool_attributes());

    if (DEBUG) puts("\nFREEING INT ATTRIBUTES");
    free_environment(get_int_attributes());

    if (DEBUG) puts("\nFREEING STRING ATTRIBUTES");
    free_environment(get_string_attributes());

    if (DEBUG) puts("\nFREEING CODE ATTRIBUTES");
    free_environment(get_code_attributes());
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
    init_null_attributes();

    init_bool_attributes();
    init_int_attributes();
    init_float_attributes();

    init_string_attributes();
    init_code_attributes();
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

Kod_Object* new_native_function_object(char* name, NativeFunction callable) {
    Kod_Object* object = new_object(OBJECT_NATIVE_FUNCTION, (Environment){0});
    object->_function.name = name;
    object->_function.callable = callable;
    object->ref_count = 1;
    debug_print("new native function object at %p - %s\n", object, name);
    return object;
}

Kod_Object *get_null_object(ConstObjectPool *cop) {
    for (size_t i = 0; i < cop->size; ++i) {
        if (cop->data[i].type == OBJECT_NULL) {
            ref_object(cop->data + i);
            return cop->data + i;
        }
    }
    return new_null_object();
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
            if (DEBUG) print_code(&object->_code, "\n", NULL, NULL);
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
