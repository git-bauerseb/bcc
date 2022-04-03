#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

#include "definitions.h"

/*
    Given a primitive type (e.g. int) returns
    the appropiate pointer.
*/
int pointer_to(int type);

/*
    Given a pointer type, return the type
    which it points to.
*/
int value_at(int type);

#endif