#ifndef C_COMPILER_TEST_H
#define C_COMPILER_TEST_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "types.h"
#include "definitions.h"

#define TEST_MSG_LENGTH 256

void test_types();

void report_test_failed(const char* msg, ...);

#endif //C_COMPILER_TEST_H
