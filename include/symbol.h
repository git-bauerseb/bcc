#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "error.h"

#define NUM_SYMBOLS 1024

enum {
    S_VARIABLE, S_FUNCTION, S_ARRAY
};

enum {
    C_GLOBAL = 1,   // Globally visible
    C_LOCAL         // Locally visible
};

typedef struct symbol_table {
    char* name;
    int endlabel;           // For S_FUNCTION, the end label
    int type;               // Primitive type for symbol
    int stype;              // Structural type for symbol
    int size;               // Number of elements in the symbol
    int class;              // Storage class (GLOBAL/LOCAL)
    int posn;               // For local variables, the relative offset from
                            // the stack base pointer

} t_symbol_entry;

static int num_globals = 0;

int add_global(char* name, int type, int stype, int endlabel, int size, int class, int posn);
int add_local(char* name, int type, int stype, int endlabel, int size, int class, int posn);

extern t_symbol_entry* sym_table;
extern int global_next_pos;
extern int local_next_pos;

#endif