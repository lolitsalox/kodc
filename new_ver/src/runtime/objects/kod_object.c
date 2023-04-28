#include "kod_object.h"
#include "kod_object_type.h"

Status kod_object_ref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't increase ref count to a null object")
    
    ++self->ref_count;
    #ifdef DEBUG_VM
    LOG_ARGS("(+) Referencing object (%d -> %d)", self->ref_count - 1, self->ref_count);
    if (self->type->str) {
        char* buffer = NULL;
        Status s = self->type->str(self, &buffer);
        if (s.type == ST_FAIL) return s;

        printf("\t('%s' - %s)", self->type->tp_name, buffer);
        free(buffer);
    }
    puts("");
    #endif
    RETURN_STATUS_OK
}

Status kod_object_deref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't decrease ref count to a null object")
    if (self->ref_count < 0) RETURN_STATUS_FAIL("Can't decrease ref count below 0")

    --self->ref_count;
    #ifdef DEBUG_VM
    LOG_ARGS("(-) Dereferencing object (%d -> %d)", self->ref_count + 1, self->ref_count);
    if (self->type->str) {
        char* buffer = NULL;
        Status s = self->type->str(self, &buffer);
        if (s.type == ST_FAIL) return s;

        printf("\t('%s' - %s)", self->type->tp_name, buffer);
        free(buffer);
    }
    puts("");
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
    LOG("(-) Freeing object");
    if (self->type->str) {
        char* buffer = NULL;
        Status s = self->type->str(self, &buffer);
        if (s.type == ST_FAIL) return s;

        printf("\t\t('%s' - %s)", self->type->tp_name, buffer);
        free(buffer);
    }
    puts("");
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