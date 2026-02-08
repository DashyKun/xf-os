#ifndef STRFN_H
#define STRFN_H

#include "types.h"

int strcmp(const char* string1, const char* string2);
bool strpfx(const char* string, const char* prefix);
int atoi(const char* str);

char** strtkn(const char* str, int* token_count);
void free_tokens(char** tokens, int token_count);

uint32_t strlen(const char* str);
char* strdup(const char* str);
char* strcat(const char* s1, const char* s2);

#endif //STRING_H