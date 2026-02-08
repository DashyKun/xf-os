#ifndef IO_H
#define IO_H

#include <stdarg.h>
#include <stddef.h>
#include "io.h"
#include "types.h"

#define VIDEO_MEMORY    ((volatile uint16_t*)0xB8000)
#define VGA_WIDTH       80
#define VGA_HEIGHT      25

/* enum for VGA colors */
typedef enum {
    BLACK = 0x0,
    BLUE = 0x1,
    GREEN = 0x2,
    CYAN = 0x3,
    RED = 0x4,
    MAGENTA = 0x5,
    BROWN = 0x6,
    WHITE = 0x7,
    GRAY = 0x8,
    LIGHT_BLUE = 0x9,
    LIGHT_GREEN = 0xA,
    LIGHT_CYAN = 0xB,
    LIGHT_RED = 0xC,
    LIGHT_MAGENTA = 0xD,
    YELLOW = 0xE,
    BRIGHT_WHITE = 0xF
} vga_color_t;

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

extern void refreshcursor(void);
extern void scroll(void);
extern void clear(void);

extern void cputc(const char c, uint8_t fg, uint8_t bg);
extern void cprintstr(const char* string, uint8_t fg, uint8_t bg);

extern void putc(const char c);
extern void printstr(const char* string);
extern void printint(size_t num);

//extern void printf(char* format, ...);

#endif //IO_H