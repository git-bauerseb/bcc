#include "../include/error.h"

void report_error(char* msg) {
    fprintf(stderr, msg);
    exit(1);
}