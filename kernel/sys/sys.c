#include "sys.h"
#include "types.h"

#define LCG_MULTIPLIER      1103515245
#define LCG_INCREMENT       12345
#define LCG_MODULUS         0x7FFFFFFF

void halt(void)
{
    while (true) {
        asm volatile("hlt");
    }
}


static uint32_t rseed = 12345;

void srand(uint32_t seed)
{
    rseed = seed;
}

uint32_t rand(void)
{
    rseed = (LCG_MULTIPLIER * rseed + LCG_INCREMENT) & LCG_MODULUS;
    return rseed;
}

uint32_t rand_range(uint32_t max)
{
    if (max == 0) return 0;
    return rand() % max;
}