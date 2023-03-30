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

#include "vm.h"
void free_object_stack(ObjectStack* stack, ConstObjectPool* cop) {
    ObjectNode* node = stack->top;
    ObjectNode* down = NULL;

    while (node) {
        down = node->down;
        debug_print("!!! object from type %s is on the stack\n", object_type_to_str(node->object->type));
        
        bool found = false;
        for (size_t i = 0; !found && (i < cop->size); ++i) {
            if (node->object == &cop->data[i]) found = true;
        }
        if (!found) deref_object(node->object);
        free(node);
        node = down;
    }
    stack->top = NULL;
}

Kod_Object* object_stack_top(ObjectStack* stack) {
    if (!stack || !stack->top) return NULL;
    return stack->top->object;
}

Kod_Object* object_stack_pop(ObjectStack* stack) {
    Kod_Object* top = object_stack_top(stack);
    if (!top) {
        puts("CANT POP FROM AN EMPTY STACK");
        return NULL;
    }
    ObjectNode* down = stack->top->down;
    free(stack->top);
    stack->top = down;
    return top;
}

void object_stack_push(ObjectStack* stack, Kod_Object* object) {
    if (!object) return;
    ObjectNode* new_node = malloc(sizeof(ObjectNode));
    new_node->object = object;
    new_node->down = stack->top;
    stack->top = new_node;
    debug_print("pushing object %s to stack\n", object_type_to_str(object->type));
}