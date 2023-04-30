#include "builtins.h"

#include "objects/tuple_object.h"
#include "objects/null_object.h"
#include "objects/str_object.h"
#include "objects/int_object.h"
#include "vm.h"

Status native_print(VirtualMachine* vm, KodObject *args, KodObject *kwargs, KodObject **out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null")

    KodObjectTuple* tuple = (KodObjectTuple*)args;
    char* buffer = NULL;
    Status s;

    for (i64 i = 0; i < tuple->size; ++i) {
        KodObject* obj = tuple->data[i];
        if (obj->type->str) {
            if ((s = obj->type->str(obj, &buffer)).type == ST_FAIL) return s;
            if (buffer) {
                printf("%s", buffer);
                free(buffer);
            }
        } else if (obj->type->repr) {
            if ((s = obj->type->repr(obj, &buffer)).type == ST_FAIL) return s;
            if (buffer) {
                printf("%s", buffer);
                free(buffer);
            }
        } else {
            if (obj->type->base.type->str) {
                if ((s = obj->type->base.type->str(AS_OBJECT(obj->type), &buffer)).type == ST_FAIL) return s;
                if (buffer) {
                    printf("%s", buffer);
                    free(buffer);
                }
            }
        }

        if (i != tuple->size - 1) printf(" ");
    }
    puts("");
    return kod_object_new_null((KodObjectNull**)out);
}


Status native_globals(VirtualMachine* vm, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null");

    object_map_print(&vm->globals);

    return kod_object_new_null((KodObjectNull**)out);
}

Status native_locals(VirtualMachine* vm, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null");

    CallFrame* curr_frame = NULL;
    Status s = frame_stack_top(&vm->frame_stack, &curr_frame);
    if (s.type == ST_FAIL) return s;

    object_map_print(&curr_frame->locals);

    return kod_object_new_null((KodObjectNull**)out);
}

Status native_input(VirtualMachine* vm, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null");
    
    char* buffer = calloc(1, 128);
    if (!buffer) RETURN_STATUS_FAIL("Coudln't allocate for buffer");

    if (((KodObjectTuple*)args)->size == 1) {
        char* s = NULL;
        if (!((KodObjectTuple*)args)->data[0]->type->str) RETURN_STATUS_FAIL("first argument doesn't have a str attribute");
        Status s_ = ((KodObjectTuple*)args)->data[0]->type->str(((KodObjectTuple*)args)->data[0], &s);
        if (s_.type == ST_FAIL) return s_;
        printf("%s", s);
        free(s);
    }

    fgets(buffer, 128, stdin);
    
    if (*buffer) buffer[strlen(buffer) - 1] = 0;

    Status s = kod_object_new_string(buffer, (KodObjectString**)out);
    free(buffer);
    if (s.type == ST_FAIL) return s;

    RETURN_STATUS_OK;
}
Status native_hash(VirtualMachine* vm, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null");
    if (((KodObjectTuple*)args)->size != 1) RETURN_STATUS_FAIL("hash takes only one argument");

    KodObject* obj = ((KodObjectTuple*)args)->data[0];
    if (!obj->type->hash) RETURN_STATUS_FAIL("Type is not hashable");

    size_t hash = 0;
    Status s = obj->type->hash(obj, &hash);
    if (s.type == ST_FAIL) return s;

    return kod_object_new_int((i64)hash, (KodObjectInt**)out);
}

Status native_dir(VirtualMachine* vm, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null");
    if (((KodObjectTuple*)args)->size != 1) RETURN_STATUS_FAIL("dir takes only one argument");

    object_map_print(((KodObjectTuple*)args)->data[0]->attributes);

    return kod_object_new_null((KodObjectNull**)out);
}


static KodObjectNativeFunc native_funcs[] = {
    STRUCT_BUILTIN_FUNC(print)
    STRUCT_BUILTIN_FUNC(globals)
    STRUCT_BUILTIN_FUNC(locals)
    STRUCT_BUILTIN_FUNC(input)
    STRUCT_BUILTIN_FUNC(hash)
    STRUCT_BUILTIN_FUNC(dir)
};

Status builtins_init(VirtualMachine* vm) {
    Status s;
    
    for (size_t i = 0; i < ARRAYSIZE(native_funcs); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(&native_funcs[i]))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&vm->globals, native_funcs[i].name, AS_OBJECT(&native_funcs[i]))).type == ST_FAIL) return s;
    }

    if ((s = kod_object_initialize_type()).type == ST_FAIL) return s;
    if ((s = kod_object_initialize_string()).type == ST_FAIL) return s;
    RETURN_STATUS_OK
}

Status builtins_destroy(VirtualMachine* vm) {
    Status s;
    if ((s = kod_object_destroy_type()).type == ST_FAIL) return s;
    if ((s = kod_object_destroy_string()).type == ST_FAIL) return s;
    RETURN_STATUS_OK
}