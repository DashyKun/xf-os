#include "keyboard.h"
#include "io/io.h"
#include "sh/shell.h"

/*
    Scancode to ASCII lookup table.
    (US keyboard layout)
*/
static const char scancode_to_ascii_lower[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, // left ctrl
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // left shift
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // right shift
    '*',
    0, // left alt
    ' ', // spacebar
    0, // caps lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F1-F10
    0, // num lock
    0, // scroll lock
    0, 0, 0, 0, 0, 0, 0, 0, 0, // keypad
    0, 0, 0, 0, 0, 0, 0,
};
static const char scancode_to_ascii_upper[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, 
    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
    0, 
    '*',
    0, 
    ' ',
    0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 
    0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
};

static bool shift_pressed, caps_lock, caps_lock_debounced = false;
static bool key_pressed[256] =  {false};

void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) { // left or right shift pressed
        shift_pressed = true;
        return;
    }
    if (scancode == 0xAA || scancode == 0xB6) { // left or right shift released
        shift_pressed = false;
        return;
    }
    if (scancode == 0x3A) {
        if (!caps_lock_debounced) {
            caps_lock = !caps_lock;
            caps_lock_debounced = true;
        }
        return;
    }
    if (scancode == 0xBA) {
        caps_lock_debounced = false;
        return;
    }
    if (scancode & 0x80) {
        uint8_t key = scancode & 0x7F;
        key_pressed[key] = false;
        return;
    }

    if (key_pressed[scancode]) {
        return;
    }
    key_pressed[scancode] = true;

    if (scancode < sizeof(scancode_to_ascii_lower)) {
        char c;

        if ((scancode >= 16 && scancode <= 25) ||
            (scancode >= 30 && scancode <= 38) ||
            (scancode >= 44 && scancode <= 50)) {
                bool use_upper = shift_pressed ^ caps_lock;
                c = use_upper ? scancode_to_ascii_upper[scancode] : scancode_to_ascii_lower[scancode];
            } else {
                c = shift_pressed ? scancode_to_ascii_upper[scancode] : scancode_to_ascii_lower[scancode];
            }
        if (c != 0) {
            sh_handlechar(c);
        }
    }
}