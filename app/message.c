#include "message.h"

#if defined _WIN32 || defined __CYGWIN__
    #include <windows.h>

    static int isEscapeCodeEnabled = 0;

    void enableEscapeCodes(){
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE)
            return;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode))
            return;

        if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            return;

        isEscapeCodeEnabled = 1;
    }
#endif

void info(char* message) {
    printf("\033[38;5;10m%s\033[0m\n", message);
}

void warn(char* message) {
    fprintf(stderr, "\033[38;5;11m%s\033[0m\n", message);
}

void sendMessage(char* type, char* text, const char* data) {
    #ifdef _WIN32
        if(!isEscapeCodeEnabled)
            enableEscapeCodes();
    #endif
    
    size_t size = strlen(text)+(!data? 0 : strlen(data)+1);
    char* message = calloc (size+1, 1);
    
    snprintf(message, size, "%s%s", text, data);

    if(strcmp(type, "warn") == 0)
        warn( message );
    else
        info( message );

    free(message);
}