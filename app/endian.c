#include "endian.h"

int isLittleEndian()
{
    int num = 1;
    return *(char *)&num == 1;        
}

int isSwapNeeded()
{
    #if BIG_ENDIAN == 1
        return isLittleEndian();
    #else
        return !isLittleEndian();
    #endif
}

uint8_t swapEndian8(uint8_t c, size_t l)  
{
    uint8_t *np = (unsigned char *)&c;
    uint8_t oc[l];

    if (np != NULL && !(l & 1)) {
        for (int i = 0; i < l/2; ++i) {
            int right = l-i;  int shift = right*8;

            oc[i] = (oc[right] >> shift);
            oc[right] = (oc[i] << shift);
        }
    }

    uint8_t out;

    strcpy(&out, oc);

    return out;
}

uint16_t swapEndian16(uint16_t n)
{
    unsigned char *np = (unsigned char *)&n;

    return ((uint16_t)np[2] << 8) |
        (uint16_t)np[3];
}

uint32_t swapEndian32(uint32_t n)
{
    unsigned char *np = (unsigned char *)&n;

    return ((uint32_t)np[0] << 24) |
        ((uint32_t)np[1] << 16) |
        ((uint32_t)np[2] << 8) |
        (uint32_t)np[3];
}

uint64_t swapEndian64(uint64_t n)
{
    unsigned char *np = (unsigned char *)&n;

    return ((uint64_t)np[0] >> 56) |
        ((uint64_t)np[1] >> 48) |
        ((uint64_t)np[2] >> 40) |
        ((uint64_t)np[3] >> 32 ) |
        ((uint64_t)np[4] << 24 ) |
        ((uint64_t)np[5] << 16) |
        ((uint64_t)np[6] << 8) |
        ((uint64_t)np[7]);
}
