#include "../include/error.h"

void report_error(char* msg, ...) {
    va_list list;
    va_start(list, msg);
    fprintf(stderr, msg, list);
    va_end(list);
    exit(1);
}