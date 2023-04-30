#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

KodStringObject* string_repr(KodStringObject* self) {
    return As_String(ref_object(As_Object(self)));
}

KodStringObject* string_new_from_string(char* str) {
    KodStringObject* result = malloc(sizeof(KodStringObject));
    result->object.ref_count = 1;
    result->object.type = OBJECT_STRING;
    result->object.type_object = &KodType_String;

    if (!str) return result;
    
    // Copy the string
    size_t size = strlen(str) + 1;
    result->_string = malloc(size * sizeof(char));
    strncpy(result->_string, str, size);

    return result;
}

KodStringObject* string_new(KodTypeObject* tp, KodObject* args, KodObject* kwargs) {
    KodStringObject* result = NULL;
    
    if (Is_Type(args, &KodType_String)) {
        KodStringObject* str_obj = (KodStringObject*)args;
        
        // Temp
        if (str_obj->object.ref_count == 0) {
            return string_new_from_string(str_obj->_string);
        }

        return As_String(ref_object(As_Object(str_obj)));
    }
    
    return result;
}

KodObject* string_add(KodStringObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

int string_bool(KodStringObject* a) {
    return strlen(a->_string);
}

KodObject* string_mul(KodStringObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

size_t string_hash(KodStringObject* a) {
    size_t hash = 5381;
    const char* c = a->_string;
    while (*c) {
        hash = ((hash << 5) + hash) + *c;
        c++;
    }
    return hash;
}

#define string_doc \
"string object, idk what else"

Kod_NumberMethods Kod_StringAsNumber = {
    .kod_abs=0,
    .kod_add=(binaryfunc)string_add,
    .kod_and=0,
    .kod_bool=(boolfunc)string_bool,
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

KodTypeObject KodType_String = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="string",
    .methods=0,
    .as_number=&Kod_StringAsNumber,
    .call=0,
    .hash=(hashfunc)string_hash,
    .repr=(methfunc)string_repr,
    .new=(typefunc)string_new,
    .free=deref_object,
    .doc=string_doc,
    .eq=0,
    .size=sizeof(char*),
};
