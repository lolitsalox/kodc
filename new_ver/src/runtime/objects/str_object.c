#include "str_object.h"
#include "int_object.h"
#include "float_object.h"
#include "bool_object.h"
#include "tuple_object.h"
#include "native_object.h"

Status str_str(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = _strdup(((KodObjectString*)self)->_string);
    if (!*out) RETURN_STATUS_FAIL("strdup failed");

    RETURN_STATUS_OK
}

Status str_repr(KodObject* self, char** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    KodObjectString* so = ((KodObjectString*)self);

    char* buffer = malloc(so->_len + 3); // '%s'
    if (!buffer) RETURN_STATUS_FAIL("Couldn't allocate for buffer");
    
    char quote = strchr(so->_string, '\'') == NULL ? '\'' : '"';

    snprintf(buffer, so->_len + 3, "%c%s%c", quote, so->_string, quote);

    *out = buffer;
    RETURN_STATUS_OK
}

Status str_add(KodObject* self, KodObject* other, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (other->type != &KodType_String) RETURN_STATUS_FAIL("other's type is not str");
 
    KodObjectString* s = (KodObjectString*)self, *o = (KodObjectString*)other;

    char* buffer = malloc(s->_len + o->_len + 1);
    if (!buffer) RETURN_STATUS_FAIL("Coudln't allocate for buffer");
        
    snprintf(buffer, s->_len + o->_len + 1, "%s%s", s->_string, o->_string);

    return kod_object_new_string(buffer, (KodObjectString**)out);
}

Status str_mul(KodObject* self, KodObject* other, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!other) RETURN_STATUS_FAIL("Invalid other");
    if (other->type != &KodType_Int && other->type != &KodType_Bool) RETURN_STATUS_FAIL("Can't multiple string by a non-int");
    
    KodObjectString* so = (KodObjectString*)self;
    i64 mul = 0;
    Status s = other->type->as_number->_int(other, &mul);
    if (s.type == ST_FAIL) return s;
    if (mul < 0) RETURN_STATUS_FAIL("Can't multiple string by a negative value");

    char* buffer = calloc(1, mul * so->_len + 1);
    if (!buffer) RETURN_STATUS_FAIL("Failed to allocate for string");

    for (i64 i = 0; i < mul; ++i) {
        memcpy(buffer + i * so->_len, so->_string, so->_len + 1);
    }

    return kod_object_new_string(buffer, (KodObjectString**)out);
}

Status str_bool(KodObject* self, bool* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = (bool)((KodObjectString*)self)->_len;
    RETURN_STATUS_OK
}

static Status str_free(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    KodObjectString* so = (KodObjectString*)self;

    if (so->_len) {
        free(so->_string);
        so->_string = "freed";
    }

    so->_len = 0;

    Status s = object_map_clear(self->attributes);
    if (s.type == ST_FAIL) return s;

    return kod_object_free(self);
}

Status str_new(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    if (!args) RETURN_STATUS_FAIL("Invalid args");
    if (args->type != &KodType_Tuple) RETURN_STATUS_FAIL("args is not a tuple");
    if (((KodObjectTuple*)args)->size != 1) RETURN_STATUS_FAIL("args size doesn't match params size");
    
    KodObject* obj = ((KodObjectTuple*)args)->data[0];
    if (!obj->type->str) RETURN_STATUS_FAIL("Type has not str attribute");
    
    char* buffer = NULL;
    Status s = obj->type->str(obj, &buffer);
    if (s.type == ST_FAIL) return s;
    s = kod_object_new_string(buffer, (KodObjectString**)out);
    free(buffer);
    return s;
}

Status str_hash(KodObject* self, size_t* out) {
    if (!self) RETURN_STATUS_FAIL("Invalid object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    *out = 5381;
    char* key = ((KodObjectString*)self)->_string;
    int c;
    while ((c = *key++) != '\0') {
        *out = (((*out) << 5) + *out) + c;  // djb2 hash algorithm
    }

    RETURN_STATUS_OK;
}

KodObjectNumberMethods str_as_number = {
    .add = str_add,
    .mul = str_mul,

    ._bool = str_bool,
};

KodObjectType KodType_String = {
    TYPE_HEADER("str")
    .as_number = &str_as_number,
    .as_subscript = 0,
    .str = str_str,
    .repr = str_repr,
    .hash = str_hash,
    .call = 0,
    .new = str_new,
    .free = str_free
};

Status native_upper(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!args) RETURN_STATUS_FAIL("Invalid args");
    if (!out) RETURN_STATUS_FAIL("Invalid out");

    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");

    KodObjectTuple* tuple = (KodObjectTuple*)args;
    KodObjectString* obj = NULL;
    
    if (tuple->size == 0) RETURN_STATUS_FAIL("args are empty");
    if (tuple->data[0] == AS_OBJECT(&KodType_String)) {
        if (tuple->size != 2 || tuple->data[1]->kind != OBJECT_STRING) RETURN_STATUS_FAIL("method takes only 1 argument from type str");
        obj = (KodObjectString*)tuple->data[1];
    }
    else {
        obj = (KodObjectString*)tuple->data[0];
    }

    Status s = kod_object_new_string(obj->_string, (KodObjectString**)out);
    if (s.type == ST_FAIL) return s;

    KodObjectString* str = (KodObjectString*)(*out);
    for (size_t i = 0; i < str->_len; ++i) {
        str->_string[i] = toupper(str->_string[i]);
    }
    RETURN_STATUS_OK;
}

static Status native___str__(VirtualMachine* vm, KodObject* self, KodObject* args, KodObject* kwargs, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("Invalid self");
    if (!args) RETURN_STATUS_FAIL("Invalid args");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    if (args->kind != OBJECT_TUPLE) RETURN_STATUS_FAIL("args is not a tuple");
    if (((KodObjectTuple*)args)->size != 2 || ((KodObjectTuple*)args)->data[1]->kind != OBJECT_STRING) RETURN_STATUS_FAIL("__str__ takes only one argument from type 'str'");

    char* buffer = NULL;
    KodObject* first_arg = ((KodObjectTuple*)args)->data[1];
    if (!first_arg->type->str) RETURN_STATUS_FAIL("the argument doesn't have a __str__ attribute");
    Status s = first_arg->type->str(first_arg, &buffer);
    if (s.type == ST_FAIL) return s;

    s = kod_object_new_string(buffer, (KodObjectString**)out);
    free(buffer);
    return s;
}

static KodObjectNativeMethod native_methods[] = {
    STRUCT_BUILTIN_METHOD(upper)
    STRUCT_BUILTIN_METHOD(__str__)
};

static KodObjectString string_name = {
    .base = {
        .kind = OBJECT_STRING,
        .ref_count = 1,
        .type = &KodType_String,
        .attributes = NULL,
    },
    ._string = "str",
    ._len = sizeof("str") - 1
};

static ObjectEntry native_names[] = {
    {.key = "__name__", .value = AS_OBJECT(&string_name)},
};


Status kod_object_new_string(char* value, KodObjectString** out) {
    KodObjectString* obj = malloc(sizeof(KodObjectString));
    if (!obj) RETURN_STATUS_FAIL("Couldn't allocate for string object");
    if (!out) RETURN_STATUS_FAIL("Invalid out");
    Status s;

    obj->_string = _strdup(value);
    if (!obj->_string) RETURN_STATUS_FAIL("strdup failed");
    obj->_len = strlen(obj->_string);
    obj->base.kind = OBJECT_STRING;
    obj->base.type = &KodType_String;
    obj->base.ref_count = 0;
    obj->base.attributes = NULL;
    if ((s = object_map_new(&obj->base.attributes)).type == ST_FAIL) return s;

    for (size_t i = 0; i < ARRAYSIZE(native_methods); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
        if ((s = object_map_insert(obj->base.attributes, native_methods[i].name, AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
    }

    *out = obj;
    RETURN_STATUS_OK
}

Status kod_object_initialize_string() {
    Status s = object_map_new(&KodType_String.base.attributes);
    if (s.type == ST_FAIL) return s;


    for (size_t i = 0; i < ARRAYSIZE(native_methods); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
        if ((s = object_map_insert(KodType_String.base.attributes, native_methods[i].name, AS_OBJECT(&native_methods[i]))).type == ST_FAIL) return s;
    }

    //ObjectMap* parent_attr = KodType_String.base.type->base.attributes;
    //if (!parent_attr) RETURN_STATUS_FAIL("type has no attributes");

    //for (size_t i = 0; i < parent_attr->capacity; ++i) {
    //    ObjectEntry* obj = parent_attr->table[i];
    //    if (!obj) continue;

    //    while (obj) {
    //        if ((s = kod_object_ref(obj->value)).type == ST_FAIL) return s;
    //        if ((s = object_map_insert(KodType_String.base.attributes, obj->key, obj->value)).type == ST_FAIL) return s;

    //        obj = obj->next;
    //    }
    //}

    for (size_t i = 0; i < ARRAYSIZE(native_names); ++i) {
        if ((s = kod_object_ref(AS_OBJECT(native_names[i].value))).type == ST_FAIL) return s;
        if ((s = object_map_insert(KodType_String.base.attributes, native_names[i].key, AS_OBJECT(native_names[i].value))).type == ST_FAIL) return s;
    }

    RETURN_STATUS_OK
}

Status kod_object_destroy_string() {
    return object_map_clear(KodType_String.base.attributes);
}