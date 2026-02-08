#include <stddef.h>
#include "strfn.h"
#include "types.h"
#include "mm/malloc.h"

int strcmp(const char* string1, const char* string2)
{
    while (*string1 && (*string1 == *string2)) {
        string1++;
        string2++;
    }
    return *(uint8_t*)string1 - *(uint8_t*)string2;
}

bool strpfx(const char* string, const char* prefix)
{
    int i = 0;
    while (prefix[i] != '\0') {
        if (string[i] == '\0' || string[i] != prefix[i]) {
            return false;
        }
        ++i;
    }
    return true;
}

int atoi(const char* str)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    while (str[i] == ' ' || str[i] == '\t') {
        i++;
    }

    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9') {
        result = result * 10 + (str[i] - '0');
        i++;
    }
    return sign * result;
}

uint32_t strlen(const char* str)
{
    uint32_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

char* strdup(const char* str)
{
    if (str == NULL) return NULL;
    
    uint32_t len = strlen(str);
    char* copy = (char*)malloc(sizeof(char) * (len + 1));
    
    if (copy != NULL) {
        for (uint32_t i = 0; i <= len; i++) {
            copy[i] = str[i];
        }
    }
    
    return copy;
}

char* strcat_new(const char* str1, const char* str2)
{
    if (str1 == NULL) return strdup(str2);
    if (str2 == NULL) return strdup(str1);
    
    uint32_t len1 = strlen(str1);
    uint32_t len2 = strlen(str2);
    
    char* result = (char*)malloc(sizeof(char) * (len1 + len2 + 1));
    if (result == NULL) return NULL;
    
    for (uint32_t i = 0; i < len1; i++) {
        result[i] = str1[i];
    }
    for (uint32_t i = 0; i <= len2; i++) {
        result[len1 + i] = str2[i];
    }
    
    return result;
}

// NEW DYNAMIC TOKENIZER
char** strtkn(const char* str, int* token_count)
{
    if (str == NULL || token_count == NULL) {
        if (token_count) *token_count = 0;
        return NULL;
    }
    
    // Count tokens first
    int count = 0;
    bool in_token = false;
    bool quoted = false;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '"') {
            quoted = !quoted;
        } else if (str[i] == ' ' && !quoted) {
            in_token = false;
        } else if (!in_token) {
            in_token = true;
            count++;
        }
    }
    
    if (count == 0) {
        *token_count = 0;
        return NULL;
    }
    
    // Allocate array of pointers
    char** tokens = (char**)malloc(sizeof(char*) * count);
    if (tokens == NULL) {
        *token_count = 0;
        return NULL;
    }
    
    // Parse and allocate each token
    int token_idx = 0;
    int char_idx = 0;
    char temp[256];  // Temp buffer for building tokens
    quoted = false;
    
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '"') {
            quoted = !quoted;
        } else if (str[i] == ' ' && !quoted) {
            if (char_idx > 0) {
                temp[char_idx] = '\0';
                tokens[token_idx++] = strdup(temp);
                char_idx = 0;
            }
        } else {
            if (char_idx < 255) {  // Prevent overflow
                temp[char_idx++] = str[i];
            }
        }
    }
    
    // Handle last token
    if (char_idx > 0) {
        temp[char_idx] = '\0';
        tokens[token_idx++] = strdup(temp);
    }
    
    *token_count = count;
    return tokens;
}

void free_tokens(char** tokens, int count)
{
    if (tokens == NULL) return;
    
    for (int i = 0; i < count; i++) {
        if (tokens[i] != NULL) {
            free(tokens[i]);
        }
    }
    free(tokens);
}