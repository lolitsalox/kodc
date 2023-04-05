#include "tuple.h"

#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

KodStringObject* tuple_repr(KodTupleObject* self) {
    char str[MAX_REPR_BUFFER_SIZE] = { 0 };
    snprintf(str, MAX_REPR_BUFFER_SIZE, "<tuple object at %p>", self);
    return string_new_from_string(str);
}

KodTupleObject* tuple_new(KodTypeObject* tp, KodObject* args, KodObject* kwargs) {
    if (!Is_Type(tp, &KodType_Tuple)) {
        UNIMPLEMENTED;
    }
    
    if (Is_Null(args)) {
        // items and size are zeroed 
        KodTupleObject* new_tuple = calloc(1, sizeof(KodTupleObject));
        ref_object(As_Object(new_tuple));
        new_tuple->object.type = OBJECT_TUPLE;
        new_tuple->object.type_object = &KodType_Tuple;
        
        return new_tuple;
    }

    UNIMPLEMENTED;
    KodTupleObject* result = NULL;
    return result;
}

size_t tuple_hash(KodTupleObject* a) {
    size_t hash = 5381;
    size_t size = a->_tuple.size;
    for (size_t i = 0; i < size; ++i) {
        hash = ((hash << 5) + hash) + (size_t)a->_tuple.items[i];
    }
    return hash;
}

int tuple_bool(KodTupleObject* a) {
    return a->_tuple.size != 0;
}

#define tuple_doc \
"tuple object"

Kod_NumberMethods Kod_TupleAsNumber = {
    .kod_abs=0,
    .kod_add=0,
    .kod_and=0,
    .kod_bool=(boolfunc)tuple_bool,
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

KodTypeObject KodType_Tuple = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="tuple",
    .methods=0,
    .as_number=&Kod_TupleAsNumber,
    .call=0,
    .hash=(hashfunc)tuple_hash,
    .repr=(methfunc)tuple_repr,
    .new=(typefunc)tuple_new,
    .free=deref_object,
    .doc=tuple_doc,
    .size=sizeof(Tuple),
};
