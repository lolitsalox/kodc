#pragma once

#include "kod_object.h"

#define MAX_STACK_SIZE 256

typedef struct ObjectStack {
    KodObject* stack[MAX_STACK_SIZE];
    i32 size;
} ObjectStack;

Status object_stack_init(ObjectStack* stack);
Status object_stack_push(ObjectStack* stack, KodObject* obj);
Status object_stack_pop(ObjectStack* stack, KodObject** out);
Status object_stack_top(ObjectStack* self, KodObject** out);
Status object_stack_clear(ObjectStack* stack, bool deref_objects);