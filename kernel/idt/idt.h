#ifndef IDT_H
#define IDT_H

#include "types.h"

extern void idt_init(void);
extern void idt_load(uint32_t);
extern void isr_handler(uint32_t int_no);

#endif //IDT_H