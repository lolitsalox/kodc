#pragma once

#include "../../compiler/compiler.h"
#include "kod_object.h"
#include "object_map.h"
#include "object_stack.h"

#define FRAME_STACK_START_CAPACITY 32
#define MAX_FRAME_STACK_SIZE 400

typedef struct CallFrame {
    ObjectMap locals;
    size_t ip;
    Code* code;
} CallFrame;

typedef struct FrameStack {
    CallFrame* frames;
    i32 size;
    u64 capacity;
} FrameStack;

Status call_frame_init(CallFrame* frame, Code* code);
Status call_frame_clear(CallFrame* frame);

Status frame_stack_init(FrameStack* stack);
Status frame_stack_push(FrameStack* stack, CallFrame frame);
Status frame_stack_pop(FrameStack* stack, CallFrame** out);
Status frame_stack_top(FrameStack* stack, CallFrame** out);
Status frame_stack_clear(FrameStack* stack);
