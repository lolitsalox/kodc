#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct linked_list_node_t {
    void* item;
    struct linked_list_node_t* next;
} linked_list_node_t;

typedef struct linked_list_t {
    linked_list_node_t* head;
    linked_list_node_t* tail;
    size_t size;
} linked_list_t;

void linked_list_init(linked_list_t* list);
void linked_list_append(linked_list_t* list, void* item);
void linked_list_remove_if(linked_list_t* list, bool (*condition)(void* item));
void linked_list_remove(linked_list_t* list, void* item);
void linked_list_foreach(linked_list_t* list, void (*loop)(void* item));
void linked_list_print(linked_list_t* list, void (*print)(void* item));