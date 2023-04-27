#include "kod_object.h"
#include "kod_object_type.h"

Status kod_object_ref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't increase ref count to a null object")
    
    ++self->ref_count;
    #ifdef DEBUG_VM
    LOG_ARGS("(+) Referencing '%s' object (%d -> %d)\n", self->type->tp_name, self->ref_count - 1, self->ref_count);
    #endif
    RETURN_STATUS_OK
}

Status kod_object_deref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't decrease ref count to a null object")
    if (self->ref_count < 0) RETURN_STATUS_FAIL("Can't decrease ref count below 0")

    --self->ref_count;
    #ifdef DEBUG_VM
    LOG_ARGS("(-) Dereferencing '%s' object (%d -> %d)\n", self->type->tp_name, self->ref_count + 1, self->ref_count);
    #endif
    if (self->ref_count <= 0) {
        if (self->type->free)
            return self->type->free(self);

        RETURN_STATUS_FAIL("Type has no free function!")
    }
    
    RETURN_STATUS_OK
}

Status kod_object_free(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't free a null object")
    #ifdef DEBUG_VM
    LOG_ARGS("(-) Freeing '%s' object\n", self->type->tp_name);
    #endif

    free(self);
    RETURN_STATUS_OK
}

u64 string_hash(char* key) {
    u64 hash = 5381;
    int c;
    while ((c = *key++) != '\0') {
        hash = ((hash << 5) + hash) + c;  // djb2 hash algorithm
    }
    return hash;
}