#include "file.h"

char* get_realpath(const char* rel){
    char* real = calloc (PATH_MAX, 1);

    #if defined _WIN32 || defined __CYGWIN__
        GetFullPathName(rel, PATH_MAX, real, NULL);
    #else
        real = realpath(rel, NULL);
    #endif

    return real;
}

void fwrite_uint8(uint8_t data, FILE* fd) {
    fwrite(&data, 1, 1, fd);
}

void fwrite_uint16(uint16_t data, FILE* fd) {
    if(isSwapNeeded())
        data = swapEndian16(data);

    fwrite(&data, 2, 1, fd);
}

void fwrite_uint32(uint32_t data, FILE* fd) {
    if(isSwapNeeded())
        data = swapEndian32(data);

    fwrite(&data, 4, 1, fd);
}

void fwrite_uint64(uint64_t data, FILE* fd) {
    if(isSwapNeeded())
        data = swapEndian64(data);

    fwrite(&data, 8, 1, fd);
}

//------------------------------------------------------------

void fwrite_time_win(time_t data, FILE* fd) {
    struct tm * utc = gmtime(&data);    //convert to utc
    long long int wtime = mktime(utc);   //save to long long int

    wtime += 11644473600LL;  //add number of seconds between epochs
    wtime *= 10000000;       //convert from seconds intervals to 100ns

    if(isSwapNeeded())
        wtime = swapEndian64(wtime);

    fwrite(&wtime, 8, 1, fd);
}

void fwrite_time_fat(time_t data, FILE* fd) {
    struct tm * utc = gmtime(&data);    //convert to utc

    unsigned long ftime = ((utc->tm_year - 80) << 25) | 
        ((utc->tm_mon+1) << 21) |
        (utc->tm_mday << 16) |
        (utc->tm_hour << 11) |
        (utc->tm_min << 5) |
        (utc->tm_sec >> 1);

    if(isSwapNeeded())
        ftime = swapEndian64(ftime);

    fwrite(&ftime, 4, 1, fd);
}

void fwrite_char(uint8_t data, size_t length, size_t count, FILE* fd) {
    if(isSwapNeeded())
        data = swapEndian64(data);

    uint8_t* buff = calloc(length*count, 1);
    buff = memmove(buff, &data, strlen(&data));
        
    fwrite(buff, length, count, fd);

    free(buff);
}