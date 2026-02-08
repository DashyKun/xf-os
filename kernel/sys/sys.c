#include "sys.h"
#include "types.h"

void halt(void)
{
    while (true) {
        asm volatile("hlt");
    }
}