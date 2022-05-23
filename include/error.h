#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdlib.h>

/*
    Definitions used for displaying errors to the user
    (e.g. language violations) and exiting.
*/

// Reports an error and exits
void report_error(char* msg);

#endif