#ifndef _PANIC_H
#define _PANIC_H

#include <stddef.h>
#include "types.h"

#define PANIC_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            panic(__FILE__, __LINE__, "ASSERT", #cond); \
        } \
    } while (false)

void panic(const char* file, uint32_t line, const char* type, const char* description);

#endif //_PANIC_H