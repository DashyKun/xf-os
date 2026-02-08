#include "io.h"

static int position = 0;

void refreshcursor(void)
{
    uint16_t pos = position;

    // low byte
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    // high byte
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void scroll(void)
{
    for (int y = 0; y < VGA_HEIGHT - 1; ++y) {
        for (int x = 0; x < VGA_WIDTH; ++x) {
            VIDEO_MEMORY[y * VGA_WIDTH + x] = VIDEO_MEMORY[(y + 1) * VGA_WIDTH + x];
        }
    }
    for (int x = 0; x < VGA_WIDTH; ++x) {
        VIDEO_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = (WHITE << 8) | ' ';
    }
    position = (VGA_HEIGHT - 1) * VGA_WIDTH;
}

void clear(void)
{
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; ++i) {
        VIDEO_MEMORY[i] = (BRIGHT_WHITE << 8) | ' ';
    }
    position = 0;
}

void cputc(const char c, uint8_t fg, uint8_t bg)
{
    uint8_t color = fg | (bg << 4);
    if (c == '\n') {
        position = (position / 80 + 1) * 80;
    } else {  
        VIDEO_MEMORY[position] = (color << 8) | c;
        ++position;
    }

    if (position >= VGA_WIDTH * VGA_HEIGHT) {
        scroll();
    }

    refreshcursor();
}

void cprintstr(const char* string, uint8_t fg, uint8_t bg)
{
    for (size_t i = 0; string[i] != '\0'; ++i) {
        cputc(string[i], fg, bg);
    }
}

void putc(const char c)
{
    if (c == '\b') {
        if (position > 0) {
            --position;
            VIDEO_MEMORY[position] = (WHITE << 8) | ' ';
            refreshcursor();
        }
    } else {
        cputc(c, WHITE, BLACK);
    }
}

void printstr(const char* string)
{
    cprintstr(string, WHITE, BLACK);
}

void printint(size_t num)
{
    char buf[32];
    int i = 0;

    if (num == 0) {
        printstr("0");
        return;
    }
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }

    buf[i] = '\0';

    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - j - 1];
        buf[i - j - 1] = tmp;
    }

    printstr(buf);
}