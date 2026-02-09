#ifndef XF_SHELL_H
#define XF_SHELL_H

#include "types.h"
#include "strfn.h"

#define INPUT_BUFFER_SIZE   256

static void sh_help(void);

typedef void    (*command_func)(void);

typedef struct {
    const char* name;
    const char* description;
    command_func func;
    bool has_args;
} command_t;

void sh_init(void);
void sh_handlechar(char c);

char sh_getc(void);
void sh_gets(char* buffer, uint32_t max_len);
#endif //XF_SHELL_H