#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct stack_t {
    void** items;
    size_t item_size;
    size_t size;
} stack_t;

void stack_init(stack_t* stack);
stack_t* stack_new();
void stack_push(stack_t* stack, void* item);
void* stack_pop(stack_t* stack);
void* stack_top(stack_t* stack);