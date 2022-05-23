#include "../include/error.h"

void report_error(char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}