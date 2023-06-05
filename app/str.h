#ifndef STR_H
#define STR_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <wchar.h>

#include "str.h"

char* str_prbrk (const char *source, const char *accept, bool nullOnNoMatch);

char *stristr(const char *haystack, const char *needle);
char *strreplace(char *string, const char *search, const char *replace, bool case_insensitive);

int str_cnt(const char *source, const char *accept);
int str_rchr(const char *source, const char *accept);

wchar_t* str_utf16(uint8_t *str);

uint8_t str_hex(char c);
uint8_t str_byte(char c1, char c2);
uint8_t* str_uuid();

void clsid_tdat(char *src, uint8_t *dst);

#endif // STR_H
