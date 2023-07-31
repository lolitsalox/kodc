#include "code.h"

#include "string.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

KodStringObject* code_repr(KodCodeObject* self) {
    char str[MAX_REPR_BUFFER_SIZE] = { 0 };
    snprintf(str, MAX_REPR_BUFFER_SIZE, "<code object '%s' at %p>", self->_code.name, self);
    return string_new_from_string(str);
}

KodCodeObject* code_new(KodTypeObject* tp, KodObject* args, KodObject* kwargs) {
    KodCodeObject* result = NULL;
    
    if (Is_Type(args, &KodType_Code)) {
        KodCodeObject* code_obj = (KodCodeObject*)args;
        
        if (code_obj->object.ref_count == 0) {
            UNIMPLEMENTED
            return result;
        }

        return (KodCodeObject*)ref_object(As_Object(code_obj));
    }
    
    return result;
}

int code_bool(KodCodeObject* a) {
    return a->_code.code != NULL;
}

#define code_doc \
"code object"

Kod_NumberMethods Kod_CodeAsNumber = {
    .kod_abs=0,
    .kod_add=0,
    .kod_and=0,
    .kod_bool=(boolfunc)code_bool,
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

KodTypeObject KodType_Code = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="code",
    .methods=0,
    .as_number=&Kod_CodeAsNumber,
    .call=0,
    .hash=0,
    .repr=(methfunc)code_repr,
    .new=(typefunc)code_new,
    .free=deref_object,
    .doc=code_doc,
    .eq=0,
    .size=sizeof(Code),
};
