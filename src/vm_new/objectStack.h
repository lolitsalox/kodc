#pragma once
#include "objects/object.h"

typedef struct ObjectNode {
    KodObject* object;
    struct ObjectNode* down;
} ObjectNode;

typedef struct ObjectStack {
    ObjectNode* top;
} ObjectStack;


void init_stack_object(ObjectStack* stack);

ObjectStack* new_stack_object();
void free_object_stack(ObjectStack* stack);

KodObject* object_stack_top(ObjectStack* stack);
KodObject* object_stack_pop(ObjectStack* stack);
void object_stack_push(ObjectStack* stack, KodObject* object);