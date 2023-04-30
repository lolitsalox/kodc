#include "type_object.h"
#include "bool_object.h"
#include "tuple_object.h"
#include "str_object.h"
#include "native_object.h"

static Status type_str_impl(KodObjectType* self, char** out) {
    if (!out) RETURN_STATUS_FAIL("out is invalid")

    char* buffer = malloc(32);
    snprintf(buffer, 32, "<type '%s'>", self->tp_name);

    *out = buffer;
    RETURN_STATUS_OK
}

static Status type_str(KodObject* self, char** out) {
    if (!out) RETURN_STATUS_FAIL("out is null");
    if (!self) RETURN_STATUS_FAIL("self is null");

    return type_str_impl((KodObjectType*)self, out);
}

static Status type_call(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("self is null");
    if (self != AS_OBJECT(&KodType_Type)) {
        if (!((KodObjectType*)self)->new) RETURN_STATUS_FAIL("type has no new method");
        return ((KodObjectType*)self)->new(vm, self, args, kwargs, out);
    }

    if (args->type != &KodType_Tuple) RETURN_STATUS_FAIL("args is not a tuple");
    if (((KodObjectTuple*)args)->size == 1) {
        KodObject* obj = ((KodObjectTuple*)args)->data[0];
        *out = AS_OBJECT(obj->type);
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("args size must be 1 for now");
}

Status type_eq(KodObject* self, KodObject* other, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    return kod_object_new_bool(self == other, (KodObjectBool**)out);
}

static Status native___str__(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!args) RETURN_STATUS_FAIL("Invalid args");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (((KodObjectTuple*)args)->size != 2 || ((KodObjectTuple*)args)->data[1]->kind != OBJECT_TYPE) RETURN_STATUS_FAIL("__str__ takes only one argument from type 'type'");
    
    char* buffer = NULL;
    KodObject* first_arg = ((KodObjectTuple*)args)->data[1];
    if (!first_arg->type->str) RETURN_STATUS_FAIL("the argument doesn't have a __str__ attribute");
    Status s = first_arg->type->str(first_arg, &buffer);
    if (s.type == ST_FAIL) return s;

    s = kod_object_new_string(buffer, (KodObjectString**)out);
    free(buffer);
    return s;
}

KodObjectType KodType_Type = {
    TYPE_HEADER("type")
    .as_number=0,
    .as_subscript=0,
    .repr=type_str,
    .str=type_str,
    .hash=0,
    .eq=type_eq,
    .call=type_call,
    .free=0,
};

static KodObjectNativeMethod native_methods[] = {
    STRUCT_BUILTIN_METHOD(__str__)
};

static KodObjectString type_name = {
    .base={
        .kind = OBJECT_STRING,
        .ref_count = 1,
        .type = &KodType_String,
        .attributes = NULL,
    },
    ._string="type",
    ._len=sizeof("type") - 1
};

static ObjectEntry native_names[] = {
    {.key = "__name__", .value = AS_OBJECT(&type_name)},
};

Status kod_object_initialize_type() {
    Status s = object_map_new(&KodType_Type.base.attributes);
    if (s.type == ST_FAIL) return s;

    for (size_t i = 0; i < ARRAYSIZE(native_methods); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
        if ((s = object_map_insert(KodType_Type.base.attributes, native_methods[i].name, AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
    }

    for (size_t i = 0; i < ARRAYSIZE(native_names); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(native_names[i].value))).type == ST_FAIL) return s;
        if ((s = object_map_insert(KodType_Type.base.attributes, native_names[i].key, AS_OBJECT(native_names[i].value))).type == ST_FAIL) return s;
    }


    RETURN_STATUS_OK;
}

Status kod_object_destroy_type() {
    return object_map_clear(KodType_Type.base.attributes);
}