#pragma once
#include "object.h"

typedef struct ObjectNode {
    Kod_Object* object;
    struct ObjectNode* down;
} ObjectNode;

typedef struct ObjectStack {
    ObjectNode* top;
} ObjectStack;


void init_stack_object(ObjectStack* stack);

ObjectStack* new_stack_object();
void free_object_stack(ObjectStack* stack);

Kod_Object* object_stack_top(ObjectStack* stack);
Kod_Object* object_stack_pop(ObjectStack* stack);
void object_stack_push(ObjectStack* stack, Kod_Object* object);