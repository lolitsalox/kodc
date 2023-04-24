#include "kod_object.h"
#include "kod_object_type.h"

Status kod_object_ref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't increase ref count to a null object")
    
    ++self->ref_count;
    RETURN_STATUS_OK
}

Status kod_object_deref(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't decrease ref count to a null object")
    
    --self->ref_count;
    if (self->ref_count <= 0) {
        if (self->type->free)
            return self->type->free(self, NULL);

        RETURN_STATUS_FAIL("Type has no free function!")
    }
}

Status kod_object_free(KodObject* self) {
    if (!self) RETURN_STATUS_FAIL("Can't free a null object")
    
    free(self);
    RETURN_STATUS_OK
}