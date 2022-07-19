#include <stdio.h>

#ifdef _WIN32
   #include <windows.h>

   void enableEscapeCodes(){
      HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
         if (hOut == INVALID_HANDLE_VALUE)
            return;

         DWORD dwMode = 0;
         if (!GetConsoleMode(hOut, &dwMode))
            return;

         if (!SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING))
            return;
   }
#endif

int main(int argc, char **argv)
{
   #ifdef _WIN32
      enableEscapeCodes();
   #endif

   printf("Hello, world.\n");
   printf("\033[38;5;8m%s\033[0m\n", "Press enter to continue...");
   getchar();
   return 0;
}