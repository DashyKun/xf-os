#include "shell.h"
#include "drivers/keyboard/keyboard.h"
#include "mm/malloc.h"
#include "io/io.h"
#include "sys/sys.h"
#include "panic/panic.h"

#define GETC_QUEUE_SIZE     16

static char getc_queue[GETC_QUEUE_SIZE];
static volatile uint32_t getc_read_pos = 0, getc_write_pos = 0;
static volatile bool getc_mode = false;
static char input_buffer[INPUT_BUFFER_SIZE];
static uint32_t buffer_index = 0;

static const char* command_args = NULL;

static command_t commands[];

static void sh_help(void)
{
    for (int i = 0; commands[i].name != 0; ++i) {
        printstr(" ");
        printstr(commands[i].name);
        printstr(" - ");
        printstr(commands[i].description);
        printstr("\n");
    }
}
static void sh_clear(void)
{
    clear();
}
static void sh_panic(void)
{
    cprintstr("Are you sure you want to invoke a panic? (y/n): ", RED, BLACK);
    char c = sh_getc();
    
    printstr("\n");
    
    if (c == 'y' || c == 'Y') {
        panic(__FILE__, __LINE__, "self-invoked", "User invoked kernel panic through shell!");
    } else {
        printstr("Panic cancelled.\n");
    }
}
static void sh_echo(void)
{
    if (command_args == NULL || *command_args == '\0') {
        printstr("DEBUG: command_args is NULL or empty\n");
        printstr("\n");
        return;
    }

    int argc;
    char** tokens = strtkn(command_args, &argc);

    bool uppercase = false;
    bool color_mode = false;
    uint8_t color = WHITE;
    int text_start = 0;

    for (int i = 0; i < argc; i++)
    {
        if (tokens[i][0] == '-') {
            if (strcmp(tokens[i], "-u") == 0) {
                uppercase = true;
                text_start = i + 1;
            }
            else if (strcmp(tokens[i], "-c") == 0 && i + 1 < argc) {
                color_mode = true;
                if (strcmp(tokens[i + 1], "red") == 0) color = RED;
                else if (strcmp(tokens[i + 1], "green") == 0) color = GREEN;
                else if (strcmp(tokens[i + 1], "blue") == 0) color = BLUE;
                else if (strcmp(tokens[i + 1], "yellow") == 0) color = YELLOW;
                text_start = i + 2;
                i++;
            }
        } else {
            break;
        }
    }

    for (int i = text_start; i < argc; i++) {
        if (uppercase) {
            for (int j = 0; tokens[i][j] != '\0'; j++) {
                char c = tokens[i][j];
                if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
                
                if (color_mode) {
                    cputc(c, color, BLACK);
                } else {
                    putc(c);
                }
            }
        } else {
            if (color_mode) {
                cprintstr(tokens[i], color, BLACK);
            } else {
                printstr(tokens[i]);
            }
        }
        
        if (i < argc - 1) {
            putc(' ');
        }
    }
    printstr("\n");
    
    free_tokens(tokens, argc);
}

static command_t commands[] = {
    {"help", "Show this message",           sh_help, false},
    {"clear", "Clears the screen",          sh_clear, false},
    {"panic", "Invokes kernel panic",       sh_panic, false},
    {"echo", "Print text to the screen",    sh_echo, true},
    {0,0,0,0}
};

static void sh_execute(void)
{
    input_buffer[buffer_index] = '\0';

    if (buffer_index == 0) {
        cprintstr("xf-os> ", YELLOW, BLACK);
        return;
    }

    for (uint32_t i = 0; commands[i].name != 0; ++i) {
        if (commands[i].has_args) {
            if (strpfx(input_buffer, commands[i].name)) {
                uint32_t cmd_len = 0;
                while (commands[i].name[cmd_len] != '\0') cmd_len++;

                command_args = input_buffer + cmd_len;
                while (*command_args == ' ') command_args++;
                
                commands[i].func();
                command_args = NULL;
                goto success;
            }
        } else {
            if (strcmp(input_buffer, commands[i].name) == 0) {
                commands[i].func();
                goto success;
            }
        }
    }
    printstr("\nUnknown command. Type 'help'.");

success:
    buffer_index = 0;
    cprintstr("\nxf-os> ", YELLOW, BLACK);
}

void sh_handlechar(char c)
{
    if (getc_mode)
    {
        if (c == '\b') {
            if (getc_write_pos != getc_read_pos) {
                getc_write_pos = (getc_write_pos - 1 + GETC_QUEUE_SIZE) % GETC_QUEUE_SIZE;
                putc('\b');
                putc(' ');
                putc('\b');
            }
            return;
        }

        uint32_t nextpos = (getc_write_pos + 1) % GETC_QUEUE_SIZE;

        if (nextpos != getc_read_pos) {
            getc_queue[getc_write_pos] = c;
            getc_write_pos = nextpos;
            putc(c);
        }
        return;
    }
    // normal shell mode begins here

    if (c == '\n') {
        printstr("\n");
        sh_execute();
    } else if (c == '\b') {
        if (buffer_index > 0) {
            --buffer_index;
            input_buffer[buffer_index] = '\0';
            putc('\b');
            putc(' ');
            putc('\b');
        }
    } else {
        if (buffer_index < INPUT_BUFFER_SIZE - 1) {
            input_buffer[buffer_index++] = c;
            putc(c);
        }
    }
}

void sh_init(void)
{
    cprintstr("\n---> xf-os <---\n", YELLOW, BLACK);
    printstr("Type 'help' for available commands.\n\n");
    cprintstr("xf-os> ", YELLOW, BLACK);
}

char sh_getc(void)
{
    // Enable interrupts (required for keyboard handler to work)
    asm volatile("sti");
    
    getc_mode = true;
    
    // Wait for a key to be pressed
    while (getc_read_pos == getc_write_pos) {
        asm volatile("nop");
    }
    
    char c = getc_queue[getc_read_pos];
    getc_read_pos = (getc_read_pos + 1) % GETC_QUEUE_SIZE;
    
    getc_mode = false;
    return c;
}

void sh_gets(char* buffer, uint32_t max_len)
{
    uint32_t i = 0;
    
    while (i < max_len - 1) {
        char c = sh_getc();
        
        if (c == '\n') {
            break;
        } else if (c == '\b') {
            if (i > 0) {
                i--;
            }
        } else {
            buffer[i++] = c;
        }
    }
    
    buffer[i] = '\0';
    printstr("\n");
}