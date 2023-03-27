#include "linked_list.h"
#include <stdlib.h>

void linked_list_init(linked_list_t* list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void linked_list_append(linked_list_t* list, void* item) {
    linked_list_node_t* new_node = malloc(sizeof(linked_list_node_t));
    new_node->item = item;
    new_node->next = NULL;
    new_node->prev = NULL;
    list->size++;

    if (!list->head) {
        list->head = new_node;
        list->tail = new_node;
        return;
    }
    
    new_node->prev = list->tail;
    list->tail->next = new_node;
    list->tail = new_node;
}

void linked_list_remove_if(linked_list_t* list, bool (*condition)(void* item)) {
    linked_list_node_t *prev = NULL, *curr = list->head;

    while (curr) {
        if (condition(curr->item)) {
            if (prev == NULL) {
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }

            list->size--;
            free(curr->item);
            free(curr);
        }
        prev = curr;
        curr = curr->next;
    }
}

void linked_list_remove(linked_list_t* list, void* item) {
    linked_list_node_t *prev = NULL, *curr = list->head;

    while (curr) {
        if (curr == item) {
            if (prev == NULL) {
                list->head = curr->next;
            } else {
                prev->next = curr->next;
            }
            
            list->size--;
            free(curr->item);
            free(curr);
        }
        prev = curr;
        curr = curr->next;
    }
}

void linked_list_foreach(linked_list_t* list, void (*loop)(void* item)) {
    linked_list_node_t* curr = list->head;
    
    while (curr) {
        loop(curr->item);
        curr = curr->next;
    }
}

void linked_list_print(linked_list_t* list, void (*print)(void* item)) {
    linked_list_foreach(list, print);
}