#include <stddef.h>
#include "malloc.h"

typedef struct block_header {
    uint32_t size;
    bool free;
    struct block_header* next;
} block_header_t;

#define BLOCK_HEADER_SIZE   sizeof(block_header_t)
#define ALIGN_SIZE          8
static block_header_t* heap_start = NULL;
static void* heap_end = NULL;

static uint32_t align(uint32_t size)
{
    return (size + (ALIGN_SIZE -1)) & ~(ALIGN_SIZE - 1);
}

void heap_init(void* start, uint32_t size)
{
    heap_start = (block_header_t*)start;
    heap_end = (void*)((char*)start+size);

    heap_start->size = size - BLOCK_HEADER_SIZE;
    heap_start->free = true;
    heap_start->next = NULL;
}

static block_header_t* find_free_block(uint32_t size)
{
    block_header_t* current = heap_start;

    while (current != NULL)
    {
        if (current->free && current->size >= size)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void split_block(block_header_t* block, uint32_t size)
{
    if (block->size >= size + BLOCK_HEADER_SIZE + ALIGN_SIZE) {
        block_header_t* new_block = (block_header_t*)((char*)block + BLOCK_HEADER_SIZE + size);
        new_block->size = block->size - size - BLOCK_HEADER_SIZE;
        new_block->free = true;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

void* malloc(uint32_t size)
{
    if (size == 0 || heap_start == NULL) {
        return NULL;
    }
    size = align(size);

    block_header_t* block = find_free_block(size);
    if (block == NULL) {
        return NULL; //out-of-memory
    }

    split_block(block, size);
    block->free = false;

    return (void*)((char*)block + BLOCK_HEADER_SIZE);
}

void free(void* ptr)
{
    if (ptr == NULL) return;

    block_header_t* block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);
    block->free = true;

    if (block->next != NULL && block->next->free) {
        block->size += BLOCK_HEADER_SIZE + block->next->size;
        block->next = block->next->next;
    }

    block_header_t* current = heap_start;
    while (current != NULL && current->next != block) {
        current = current->next;
    }
    if (current != NULL && current->free) {
        current->size += BLOCK_HEADER_SIZE + block->size;
        current->next = block->next;
    }
}

void* calloc(uint32_t num, uint32_t size)
{
    uint32_t total_size = num * size;
    void* ptr = malloc(total_size);
    
    if (ptr != NULL) {
        // zero out the memory
        char* p = (char*)ptr;
        for (uint32_t i = 0; i < total_size; i++) {
            p[i] = 0;
        }
    }
    
    return ptr;
}

void* realloc(void* ptr, uint32_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    block_header_t* old_block = (block_header_t*)((char*)ptr - BLOCK_HEADER_SIZE);

    if (align(size) <= old_block->size) {
        return ptr;
    }
    
    void* new_ptr = malloc(size);
    if (new_ptr == NULL) {
        return NULL;
    }

    char* src = (char*)ptr;
    char* dst = (char*)new_ptr;
    for (uint32_t i = 0; i < old_block->size; i++) {
        dst[i] = src[i];
    }

    free(ptr);
    
    return new_ptr;
}