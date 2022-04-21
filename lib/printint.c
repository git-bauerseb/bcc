#include <stdio.h>

void printint(long x) {
    printf("%ld\n", x);
}

void printchar(long c) {
    fputc((char)(c & 0x7f), stdout);
}