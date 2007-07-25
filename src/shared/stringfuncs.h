
#ifndef H_STRING_FUNCS
#define H_STRING_FUNCS
#include "common.h"

#define MAX_CHAR 0x7E  // Max valid char value
#define MIN_CHAR 0x20  // Min valid char value

#define isChar(a)  (a >= MIN_CHAR && a <= MAX_CHAR) // Tests for whitespace (not including spaces)
#define isCharw(a)  (a > MIN_CHAR && a <= MAX_CHAR) // Tests for whitespace (including spaces)

void strcws(char *string, bool removespaces = false);
void strrc(char *string, char remove);
#endif

