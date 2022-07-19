#ifndef ENDIAN_H
#define ENDIAN_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Little-endian needed
#define BIG_ENDIAN 0

int isLittleEndian();
int isSwapNeeded();

uint8_t swapEndian8(uint8_t c, size_t l);
uint16_t swapEndian16(uint16_t n);
uint32_t swapEndian32(uint32_t n);
uint64_t swapEndian64(uint64_t n);

#endif // ENDIAN_H
