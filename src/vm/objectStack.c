#include "objectStack.h"

#include <stdio.h>
#include <stdlib.h>


void init_stack_object(ObjectStack* stack) {
    if (!stack) return;
    stack->top = NULL;
}

ObjectStack* new_stack_object() {
    ObjectStack* stack = malloc(sizeof(ObjectStack));
    init_stack_object(stack);
    return stack;
}

void free_object_stack(ObjectStack* stack) {
    ObjectNode* node = stack->top;
    ObjectNode* down = NULL;

    while (node) {
        down = node->down;
        free(node);
        node = down;
    }
}

Kod_Object* object_stack_top(ObjectStack* stack) {
    if (!stack || !stack->top) return NULL;
    return stack->top->object;
}

Kod_Object* object_stack_pop(ObjectStack* stack) {
    Kod_Object* top = object_stack_top(stack);
    if (!top) {
        fputs("CANT POP FROM AN EMPTY STACK", stderr);
        return NULL;
    }
    ObjectNode* down = stack->top->down;
    free(stack->top);
    stack->top = down;
    return top;
}

void object_stack_push(ObjectStack* stack, Kod_Object* object) {
    ObjectNode* new_node = malloc(sizeof(ObjectNode));
    new_node->object = object;
    new_node->down = stack->top;
    stack->top = new_node;
}