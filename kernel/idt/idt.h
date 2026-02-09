#ifndef IDT_H
#define IDT_H

#include "types.h"

typedef struct {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

extern void idt_init(void);
extern void idt_load(uint32_t);
extern void isr_handler(registers_t* regs);

#endif //IDT_H