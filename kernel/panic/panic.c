#include "panic.h"
#include "sys/sys.h"
#include "io/io.h"

void panic(const char* file, uint32_t line, const char* type, const char* description)
{
    asm volatile ("cli");
    printstr("Kernel panic: ");
    printstr(file);
    printstr(":");
    printint(line);
    printstr(": ");
    printstr(type);
    printstr(" -> ");
    printstr(description);  
    halt();
}