#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

#include "definitions.h"
#include "scan.h"

// Returns true if the provided type is an integral value.
int inttype(int type);

// Given a primitive type, returns the type which is a pointer to
// this type.
int pointer_to(int type);

// Returns true if the provided type is a pointer.
int pointer_type(int type);

// Given a primitive pointer type, return the type it points to.
// E.g. int*** -> int**.
int value_at(int type);

#endif