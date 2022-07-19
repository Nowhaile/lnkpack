#ifndef MESSAGE_H
#define MESSAGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "str.h"

void info(char* message);
void warn(char* message);
void sendMessage(char* type, char* text, const char* data);

#endif // MESSAGE_H
