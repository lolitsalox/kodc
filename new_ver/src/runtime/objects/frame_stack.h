#pragma once

#include "kod_object.h"
#include "object_map.h"
#include "object_stack.h"
#include <compiler/compiler.h>

#define MAX_FRAMES 256 // somehow need to make this higher

typedef struct CallFrame {
    ObjectMap locals;
    struct ObjectStack stack;
    size_t ip;
    Code* code;
} CallFrame;

typedef struct FrameStack {
    CallFrame frames[MAX_FRAMES];
    size_t size;
} FrameStack;


Status call_frame_init(CallFrame* frame, Code* code);
Status call_frame_clear(CallFrame* frame);

Status frame_stack_init(FrameStack* stack);
Status frame_stack_push(FrameStack* stack, CallFrame frame);
Status frame_stack_pop(FrameStack* stack, CallFrame** out);
Status frame_stack_top(FrameStack* stack, CallFrame** out);
Status frame_stack_clear(FrameStack* stack);
