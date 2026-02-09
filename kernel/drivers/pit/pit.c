#include "pit.h"
#include "io/io.h"

#define PIT_CHANNEL0    0x40
#define PIT_COMMAND     0x43
#define PIT_FREQUENCY   1193182
#define TARGET_HZ       1000

static volatile uint32_t pit_ticks = 0;

void pit_init(void)
{
    uint32_t divisor = PIT_FREQUENCY / TARGET_HZ;
    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);

    pit_ticks = 0;
}

void pit_tick(void)
{
    pit_ticks++;
}

uint32_t pit_get_ticks(void)
{
    return pit_ticks;
}

void sleepms(uint32_t ms)
{
    uint32_t target = pit_ticks + ms;
    asm volatile ("sti");

    while (pit_ticks < target) {
        asm volatile ("hlt");
    }
}