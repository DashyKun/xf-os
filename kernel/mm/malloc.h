#ifndef MALLOC_H
#define MALLOC_H

#include "types.h"

void heap_init(void* start, uint32_t size);

void* malloc(uint32_t size);
void free(void* ptr);
void* calloc(uint32_t num, uint32_t size);
void* realloc(void* ptr, uint32_t size);

#endif //MALLOC_H