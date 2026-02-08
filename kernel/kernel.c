#include "io/io.h"
#include "types.h"
#include "sys/sys.h"
#include "panic/panic.h"
#include "gdt/gdt.h"
#include "idt/idt.h"
#include "sh/shell.h"
#include "mm/malloc.h"

extern char __heap_start;
extern char __heap_end;

static void kstatus_print(bool status, const char* status_to_print) // i am the best at naming variables
{
    if (status == true) {
        printstr("[  ");
        cprintstr("OK", GREEN, BLACK);
        printstr("  ]  ");
        printstr(status_to_print);
        printstr("\n");
    }
    else
    {
        printstr("[  ");
        cprintstr("FAIL", RED, BLACK);
        printstr("  ]  ");
        printstr(status_to_print);
        printstr("\n");
    }
}

void kmain(void)
{
    clear();
    gdt_init();
    kstatus_print(true, "GDT loaded!");

    idt_init();
    kstatus_print(true, "IDT loaded!");

    kstatus_print(true, "Keyboard initialized!");

    uint32_t heap_size = (uint32_t)(&__heap_end - &__heap_start);
    heap_init(&__heap_start, heap_size);
    kstatus_print(true, "Heap initialized!");

    // has to be reversed or it will cause issues with the init message
    kstatus_print(true, "Shell initialized!");
    sh_init();

    halt();
}