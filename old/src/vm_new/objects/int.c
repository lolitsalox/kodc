#include "int.h"

#include "string.h"

KodStringObject* int_repr(KodIntObject* self) {
    if (!self) {
        return As_String(KodType_Null.repr(&Kod_Null, NULL, NULL));
    }
    char str[MAX_REPR_BUFFER_SIZE] = { 0 };
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
    
    return string_new_from_string(str);
}

// what to put in params??
KodObject* int_new(KodObject* tp) {
    UNIMPLEMENTED; // return new int object
}

size_t int_hash(KodIntObject* a) {
    return a->_int;
}

#define int_doc \
"int object, idk what else"

KodObject* int_abs(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_add(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_and(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

int int_bool(KodIntObject* a) {
    return a->_int;
}

KodObject* int_div(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_floor(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_float(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_int(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_invert(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_mod(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_mul(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_neg(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_or(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_pos(KodIntObject* a) {
    UNIMPLEMENTED;
}

KodObject* int_pow(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_shl(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_shr(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_sub(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

KodObject* int_xor(KodIntObject* a, KodObject* b) {
    UNIMPLEMENTED;
}

Kod_NumberMethods Kod_IntAsNumber = {
    .kod_abs    =(unaryfunc) int_abs,
    .kod_add    =(binaryfunc)int_add,
    .kod_and    =(binaryfunc)int_and,
    .kod_bool   =(boolfunc)  int_bool,
    .kod_div    =(binaryfunc)int_div,
    .kod_float  =(unaryfunc) int_float,
    .kod_floor  =(binaryfunc)int_floor,
    .kod_int    =(unaryfunc) int_int,
    .kod_invert =(unaryfunc) int_invert,
    .kod_mod    =(binaryfunc)int_mod,
    .kod_mul    =(binaryfunc)int_mul,
    .kod_neg    =(unaryfunc) int_neg,
    .kod_or     =(binaryfunc)int_or,
    .kod_pos    =(unaryfunc) int_pos,
    .kod_pow    =(binaryfunc)int_pow,
    .kod_shl    =(binaryfunc)int_shl,
    .kod_shr    =(binaryfunc)int_shr,
    .kod_sub    =(binaryfunc)int_sub,
    .kod_xor    =(binaryfunc)int_xor
};

KodTypeObject KodType_Int = {
    .object={.ref_count=1,.type=OBJECT_TYPE,.type_object=&KodType_Type},
    .type_name="int",
    .methods=0,
    .as_number=&Kod_IntAsNumber,
    .call=0,
    .hash=(hashfunc)int_hash,
    .repr=(methfunc)int_repr,
    .new=(typefunc)int_new,
    .free=deref_object,
    .doc=int_doc,
    .eq=0,
    .size=sizeof(int64_t),
};
