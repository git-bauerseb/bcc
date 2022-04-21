#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"

#define NUM_SYMBOLS 1024

enum {
    S_VARIABLE, S_FUNCTION, S_ARRAY
};

typedef struct symbol_table {
    char* name;
    int endlabel;           // For S_FUNCTION, the end label
    int type;               // Primitive type for symbol
    int stype;              // Structural type for symbol
    int size;               // Number of elements in the symbol
} t_symbol_entry;

static int num_globals = 0;

int find_global(char* s);
int add_global(char* name, int type, int stype, int endlabel, int size);

extern t_symbol_entry* global_symbols;

#endif