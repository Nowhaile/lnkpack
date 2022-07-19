#include "str.h"

char* str_prbrk (const char *source, const char *accept, bool nullOnNoMatch){
    const char *end = source + strlen(source);

    while(end >= source) {
        const char *a = accept;
        while(*a){
            if(*a++ == *end) {
                return (char *)end+1;
            }
        }
        end--;
    }

	if(nullOnNoMatch)
    	return NULL;

	return (char *)source;
}

int str_cnt(const char *source, const char *accept){
	int count = 0;

	while (*source != '\0')
    {
      	const char *a = accept;		
      	while (*a != '\0')
          	count += (*a++ == *source);		
      	source++;
    }

	return count;
}

int str_rchr(const char *source, const char *accept){
	const char *end = source + strlen(source);

    while(end >= source) {
        const char *a = accept;
        while(*a){
            if(*a++ == *end) {
                return end-source;
            }
        }
        end--;
    }

    return -1;
}

wchar_t* str_utf16(uint8_t *str) {
	size_t wsize = mbstowcs( NULL, str, 0)+ 1;
	wchar_t* dest = calloc(wsize, sizeof( wchar_t ));	
    
    mbstowcs( dest, str, wsize);

	return dest;
}

uint8_t str_hex(char c)
{
	if (c >= 'A') {
		return (c - 'A' + 10);
	}
	else {
		return (c - '0');
	}
}

uint8_t str_byte(char c1, char c2)
{
	return str_hex(toupper(c1)) * 16 + str_hex(toupper(c2));
}

void clsid_tdat(char *src, uint8_t *dst) {
	dst[0] = str_byte(src[6], src[7]);
	dst[1] = str_byte(src[4], src[5]);
	dst[2] = str_byte(src[2], src[3]);
	dst[3] = str_byte(src[0], src[1]);
	dst[4] = str_byte(src[11], src[12]);
	dst[5] = str_byte(src[9], src[10]);
	dst[6] = str_byte(src[16], src[17]);
	dst[7] = str_byte(src[14], src[15]);
	dst[8] = str_byte(src[19], src[20]);
	dst[9] = str_byte(src[21], src[22]);
	dst[10] = str_byte(src[24], src[25]);
	dst[11] = str_byte(src[26], src[27]);
	dst[12] = str_byte(src[28], src[29]);
	dst[13] = str_byte(src[30], src[31]);
	dst[14] = str_byte(src[32], src[33]);
	dst[15] = str_byte(src[34], src[35]);
}

uint8_t* str_uuid(){	
    char buf[37] = {0};
	char v[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

	for( int i = 0; i < 36; ++i )
	{
		if( i == 8 || i == 13 || i == 18 || i == 23 )
		{
			buf[i] = '-';
		}
		else
		{
			buf[i] = v[rand()%16];
		}
	}

    buf[36] = '\0';

	static uint8_t hex[16] = {0};
	clsid_tdat(buf, hex);

	return hex;
}