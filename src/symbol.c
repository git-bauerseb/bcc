#include "../include/symbol.h"

static int new_global(void);

/*
    Update a symbol at the given index. The parameters are:

    + slot: the index in the symbol table
    + name: name of the symbol
    + type: the type of the symbol (char/short/int/long)
    + stype: structural type (variable/function/array/struct)
    + endlabel: for functions
    + posn: position information for local variables
    + class: the storage class (global/local)
*/
static void update_symbol(int slot, char* name, int type, int stype,
                          int class, int end_label, int size, int posn);


static int new_global(void) {
    int p;

    if ((p = global_next_pos++) >= local_next_pos) {
        fprintf(stderr, "Too many global symbols\n");
        exit(1);
    }

    return p;
}

int find_global(char* s) {
    int i = 0;

    for (; i < NUM_SYMBOLS; i++) {
        if (sym_table[i].name != NULL && !strcmp(s, sym_table[i].name)) {
            return i;
        }
    }

    return -1;
}

int add_local(void) {
    int p;
    if ((p = local_next_pos--) <= global_next_pos) {
        fprintf(stderr, "Too many local symbols\n");
        exit(1);
    }

    return p;
}

int add_global(char* name, int type, int stype, int endlabel, int size, int class, int posn) {
    int y;

    if ((y = find_global(name)) != -1) {
        return y;
    }

    y = new_global();
    update_symbol(y, name, type, stype, class, endlabel, size, posn);
    return y;
}

static void update_symbol(int slot, char* name, int type, int stype,
                          int class, int end_label, int size, int posn) {

    if (slot < 0 || slot > NUM_SYMBOLS) {
        report_error("Slot index is invalid.\n");
    }

    sym_table[slot].class = class;
    sym_table[slot].name = name;
    sym_table[slot].type = type;
    sym_table[slot].stype = stype;
    sym_table[slot].size = size;
    sym_table[slot].posn = posn;
    sym_table[slot].endlabel = end_label;
}