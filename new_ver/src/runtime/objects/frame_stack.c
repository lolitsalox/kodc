#include "frame_stack.h"

Status call_frame_init(CallFrame* frame, Code* code) {
    if (!frame) RETURN_STATUS_FAIL("frame is null")
    if (!code) RETURN_STATUS_FAIL("code is null")
    object_map_init(&frame->locals);
    object_stack_init(&frame->stack);
    frame->ip = 0;
    frame->code = code;
    
    RETURN_STATUS_OK
}

Status call_frame_clear(CallFrame* frame) {
    if (!frame) RETURN_STATUS_FAIL("frame is null")
    Status s;
    printf("Clearing frame\nClearing map\n");
    if ((s = object_map_clear(&frame->locals)).type == ST_FAIL) return s;
    printf("Clearing stack\n");
    if ((s = object_stack_clear(&frame->stack, true)).type == ST_FAIL) return s;
    RETURN_STATUS_OK
}

Status frame_stack_init(FrameStack* self) {
    if (!self) RETURN_STATUS_FAIL("frame stack is null")

    self->size = 0;
    RETURN_STATUS_OK
}

Status frame_stack_push(FrameStack* self, CallFrame frame) {
    if (!self) RETURN_STATUS_FAIL("frame stack is null")

    if (self->size < MAX_FRAMES) {
        self->frames[self->size++] = frame;
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Frame stack is full. Cannot push frame onto stack.");
}

Status frame_stack_pop(FrameStack* self, CallFrame** out) {
    if (!self) RETURN_STATUS_FAIL("frame stack is null")

    if (self->size > 0) {
        CallFrame* cf = &self->frames[--self->size];
        if (out) *out = cf;
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Frame stack is empty. Cannot pop frame from stack.")
}

Status frame_stack_top(FrameStack* self, CallFrame** out) {
    if (!self) RETURN_STATUS_FAIL("frame stack is null")

    if (self->size) {
        *out = &self->frames[self->size - 1];
        RETURN_STATUS_OK
    }

    RETURN_STATUS_FAIL("Frame stack is empty.")
}

Status frame_stack_clear(FrameStack* self) {
    if (!self) RETURN_STATUS_FAIL("frame stack is null")

    Status s;
    for (i32 i = self->size - 1; i >= 0 ; --i) {

        if ((s = call_frame_clear(&self->frames[i])).type == ST_FAIL) return s;
    }
    self->size = 0;
    RETURN_STATUS_OK
}
