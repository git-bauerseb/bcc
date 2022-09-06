#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// Definitions used to report error that occured either
// during compilation or at the system level.

// Reports an error and exits
void report_error(char* msg, ...);

#endif