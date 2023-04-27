#include "object_stack.h"
#include "kod_object_float.h"
#include "kod_object_int.h"

Status object_stack_init(ObjectStack* self) {
    if (!self) RETURN_STATUS_FAIL("stack is null")

    self->size = 0;
    RETURN_STATUS_OK
}

Status object_stack_push(ObjectStack* self, KodObject* obj) {
    if (!self) RETURN_STATUS_FAIL("stack is null")
    if (!obj) RETURN_STATUS_FAIL("obj is null")

    #ifdef DEBUG_VM
    LOG("(+) Pushing to object stack\n");
    #endif

    if (self->size < MAX_STACK_SIZE) {
        self->stack[self->size++] = obj;
        RETURN_STATUS_OK
    }
    
    RETURN_STATUS_FAIL("Stack is full. Cannot push object onto stack.");
}

Status object_stack_pop(ObjectStack* self, KodObject** out) {
    if (!self) RETURN_STATUS_FAIL("stack is null")
    if (!out) RETURN_STATUS_FAIL("out is null")

    #ifdef DEBUG_VM
    LOG("(-) Popping from object stack\n");
    #endif

    if (self->size > 0) {
        *out = self->stack[--self->size];
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Stack is empty. Cannot pop object from stack.")
}

Status object_stack_clear(ObjectStack* self, bool deref_objects) {
    if (!self) RETURN_STATUS_FAIL("stack is null")

    for (size_t i = 0; i < self->size; i++) {
        #ifdef DEBUG_VM
        LOG("(-) Popping from object stack\n");
        #endif

        if (deref_objects) {
            Status s = kod_object_deref(self->stack[i]);
            if (s.type == ST_FAIL) return s;
        }
        self->stack[i] = NULL;
    }
    self->size = 0;
    RETURN_STATUS_OK
}
