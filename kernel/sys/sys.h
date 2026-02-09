#ifndef SYS_H
#define SYS_H

#include "types.h"

extern void halt(void);

void srand(uint32_t seed);
uint32_t rand(void);
uint32_t rand_range(uint32_t max);

#endif //SYS_H