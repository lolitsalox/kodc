#include "builtins.h"

#include "objects/kod_object_tuple.h"
#include "objects/kod_object_null.h"

Status native_print(KodObject *args, KodObject *kwargs, KodObject **out) {
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (!out) RETURN_STATUS_FAIL("out is null")

    KodObjectTuple* tuple = (KodObjectTuple*)args;
    char* buffer = NULL;
    Status s;

    for (size_t i = 0; i < tuple->size; ++i) {
        KodObject* obj = tuple->data[i];
        if (obj->type->str) {
            if ((s = obj->type->str(obj, &buffer)).type == ST_FAIL) return s;
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

static KodObjectNativeFunc native_funcs[] = {
    STRUCT_BUILTIN_FUNC(print)
};

Status builtins_init(VirtualMachine* vm) {
    Status s;
    
    for (size_t i = 0; i < ARRAYSIZE(native_funcs); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(&native_funcs[i]))).type == ST_FAIL) return s;
        if ((s = object_map_insert(&vm->globals, native_funcs[i].name, AS_OBJECT(&native_funcs[i]))).type == ST_FAIL) return s;
    }

    RETURN_STATUS_OK
}

