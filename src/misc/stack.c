#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include "../parser/ast.h"

void stack_init(stack_t* stack) {
    if (!stack) return;
    stack->item_size = sizeof(void*);
    stack->items = NULL;
    stack->size = 0;
}

stack_t* stack_new() {
    stack_t* stack = malloc(sizeof(stack_t));
    stack_init(stack);
    return stack;
}

void stack_push(stack_t* stack, void* item) {
    if (!stack) return;
    // Incrementing size
    stack->items = realloc(stack->items, ++stack->size * stack->item_size);
    if (!stack->items) return;

    stack->items[stack->size - 1] = item;
}

void* stack_pop(stack_t* stack) {
    if (!stack) return NULL;
    if (stack->size == 0) return NULL;
    
    void* item = stack->items[stack->size - 1];
    
    // Decrementing size
    stack->items = realloc(stack->items, --stack->size * stack->item_size);

    return item;
}

void* stack_top(stack_t* stack) {
    if (!stack) return NULL;
    if (stack->size == 0) return NULL;
    
    return stack->items[stack->size - 1];
}