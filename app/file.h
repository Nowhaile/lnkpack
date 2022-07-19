#ifndef FILE_H
#define FILE_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <time.h>

#if defined _WIN32 || defined __CYGWIN__
    #include <windows.h>
#endif

#include "endian.h"

char* get_realpath(const char* rel);

void fwrite_uint8(uint8_t data, FILE* fd);
void fwrite_uint16(uint16_t data, FILE* fd);
void fwrite_uint32(uint32_t data, FILE* fd);
void fwrite_uint64(uint64_t data, FILE* fd);

void fwrite_time_win(time_t data, FILE* fd);
void fwrite_time_fat(time_t data, FILE* fd);

void fwrite_char(uint8_t data, size_t length, size_t count, FILE* fd);

#endif // FILE_H
