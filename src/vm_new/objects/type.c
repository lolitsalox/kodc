#include "type.h"

#include "string.h"

KodObject* type_repr(KodTypeObject* tp) {
    return NULL; // build new string <class 'type'> or <class 'mod.type'>
}

KodObject* type_new(KodObject* tp) {
    return NULL; // return new type object
}

size_t type_hash(KodTypeObject* a) {
    size_t hash = 5381;
    const char* c = a->type_name;
    while (*c) {
        hash = ((hash << 5) + hash) + *c;
        c++;
    }
    return hash;
}

#define type_doc \
"type object, idk what else"

KodTypeObject KodType_Type = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="type",
    .methods=0,
    .as_number=0,
    .call=0,
    .hash=(hashfunc)type_hash,
    .repr=(methfunc)type_repr,
    .new=(typefunc)type_new,
    .free=deref_object,
    .doc=type_doc,
    .size=sizeof(KodTypeObject),
};


KodObject Kod_Null = {
    .ref_count=1,
    .type=OBJECT_NULL,
    .type_object=&KodType_Null
};

KodObject* null_repr(KodObject* self) {
    return As_Object(string_new_from_string("null"));
}

KodObject* null_new() {
    return ref_object(&Kod_Null);
}

#define null_doc \
"null object"

int null_bool(KodObject* a) {
    return 0;
}

Kod_NumberMethods Kod_NullAsNumber = {
    .kod_abs=0,
    .kod_add=0,
    .kod_and=0,
    .kod_bool=(boolfunc)null_bool,
    .kod_div=0,
    .kod_float=0,
    .kod_floor=0,
    .kod_int=0,
    .kod_invert=0,
    .kod_mod=0,
    .kod_mul=0,
    .kod_neg=0,
    .kod_or=0,
    .kod_pos=0,
    .kod_pow=0,
    .kod_shl=0,
    .kod_shr=0,
    .kod_sub=0,
    .kod_xor=0
};

KodTypeObject KodType_Null = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="null",
    .methods=0,
    .as_number=&Kod_NullAsNumber,
    .call=0,
    .hash=(hashfunc)null_bool,
    .repr=(methfunc)null_repr,
    .new=(typefunc)null_new,
    .free=deref_object,
    .doc=null_doc,
    .size=sizeof(Kod_Null),
};