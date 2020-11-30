#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>

#include "rcmalloc.h"



typedef struct __node_t {
    size_t size;
    struct __node_t *next;
} node_t;

int m_error;
node_t* free_list = NULL;
void* base = NULL;


int rcinit(int size_of_region){
    base = mmap(NULL, size_of_region, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
    free_list = (node_t*) base;
    free_list->size = size_of_region - sizeof(node_t);
    free_list->next = NULL;

    return 0;
}


node_t* create_node(size_t addr, int size){
    node_t* node = (node_t*)addr;
    node->size = size - sizeof(node_t);
    node->next = NULL;

    return node;
}


void insert_node(node_t* node, node_t* new_node){
    new_node->next = node->next;
    node->next = new_node;
}


void * get_block_end_addr(node_t* node){
    return (void *)node + sizeof(node_t) + node->size;
}


void * rcmalloc(int size, int style){
    while (size % 8 != 0) size ++;

    node_t* node = free_list;
    node_t* prev = NULL;
    node_t* target_node = node;
    node_t* target_node_prev = prev;

    if ((style == M_BESTFIT) || (style == M_FIRSTFIT)) {
        while (node) {
            if (size <= node->size && node->size < target_node->size){
                target_node_prev = prev;
                target_node = node;
                if (style == M_FIRSTFIT) break;
            }
            prev = node;
            node = node->next;
        }
    } else if (style == M_WORSTFIT) {
        while (node) {
            if (size <= node->size && node->size > target_node->size){
                target_node_prev = prev;
                target_node = node;
            }
            prev = node;
            node = node->next;
        }
    }

    if (target_node->size < size) {
        m_error = E_NO_SPACE;
        return NULL;
    }

    size_t addr = (size_t)((void *)(target_node) + sizeof(node_t) + size);
    size_t size_left = target_node->size - size;

    if (size_left <= sizeof(node_t)) {
        size += size_left;
        size_left = 0;
    }

    target_node->size = size;

    if (size_left) {
        insert_node(target_node, create_node(addr, size_left));
    }

    if (target_node_prev) {
        target_node_prev->next = target_node->next;
    } else {
        free_list = target_node->next;
    }
    // printf("best_node->next: %p\n", best_node->next);
    return (void*)target_node + sizeof(node_t);
};



int rcfree(void * ptr){
    node_t* released_node = ptr - sizeof(node_t);

    node_t* node = free_list;

    while(node && node->next && (node->next < released_node)) {
        node = node->next;
    }

    node_t* node_before = NULL;
    node_t* node_after = NULL;

    if (node && node < released_node && (node->next > released_node)) {
        node_before = node;
        node_after = node_before->next;
    } else if (node && node < released_node && (node->next == NULL)) {
        node_before = node;
    } else if (node > released_node) {
        node_after = node;
    }

    // printf("node_before = %p\n", node_before);
    // printf("node_after  = %p\n", node_after);

    if (node_after) {
        released_node->next = node_after;
        if (get_block_end_addr(released_node) == (void *)node_after) {
            released_node->size += node_after->size + sizeof(node_t);
            released_node->next = node_after->next;
        }
    } else {
        released_node->next = NULL;
    }

    if (node_before){
        node_before->next = released_node;
        if (get_block_end_addr(node_before) == (void *)released_node) {
            node_before->size += released_node->size + sizeof(node_t);
            node_before->next = released_node->next;
        }
    }

    if (!node_before) free_list = released_node;

    return 0;
};


void rcdump(){
    printf("====================DUMP====================\n");
    node_t* node = free_list;
    int index = 0;
    while(node) {
        printf("node %d, size=%lu, head_addr=%p\n", index, node->size, node);
        node = node->next;
    }
};

