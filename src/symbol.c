#include "../include/symbol.h"

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

static int new_local(void) {
    int p;

    if ((p = local_next_pos--) <= global_next_pos) {
        fprintf(stderr, "Too many local symbols\n");
        exit(1);
    }

    return p;
}

static int find_global(char* s) {
    int i = 0;

    for (; i < global_next_pos; i++) {

        // Globals also can contain paramters
        if (sym_table[i].class == C_PARAMETER) {continue;}

        if (sym_table[i].name != NULL && !strcmp(s, sym_table[i].name)) {
            return i;
        }
    }

    return -1;
}

static int find_local(char* s) {
    int i;

    for (i = local_next_pos + 1; i < NUM_SYMBOLS; i++) {
        if (*s == *sym_table[i].name && !strcmp(s, sym_table[i].name))
            return i;
    }
    return -1;
}

static void update_symbol(int slot, char* name, int type, int stype,
                          int class, int end_label, int size, int posn) {

    if (slot < 0 || slot > NUM_SYMBOLS) {
        report_error("Slot index is invalid.\n");
    }

    sym_table[slot].class = class;
    sym_table[slot].name = strdup(name);        // Duplicate, otherwise it's a reference!
    sym_table[slot].type = type;
    sym_table[slot].stype = stype;
    sym_table[slot].size = size;
    sym_table[slot].posn = posn;
    sym_table[slot].endlabel = end_label;
}

int find_symbol(char* name) {
    int slot;

    if ((slot = find_global(name)) == -1) {
        slot = find_local(name);
    }

    return slot;
}

int add_local(char* name, int type, int stype, int endlabel, int size, int isparam) {
    int localslot, globalslot, posn;

    if ((localslot = find_local(name)) != -1) {
        return -1;
    }

    localslot = new_local();
    posn = cg_get_local_offset(type, 0);
    if (isparam) {
        update_symbol(localslot, name, type, stype, C_PARAMETER, 0, size, 0);
        globalslot = new_global();
        update_symbol(globalslot, name, type, stype, C_PARAMETER, 0, size, 0);
    } else {
        update_symbol(localslot, name, type, stype, C_LOCAL, 0, size, 0);
    }

    return localslot;
}

int add_global(char* name, int type, int stype, int endlabel, int size) {
    int slot;

    if ((slot = find_global(name)) != -1) {
        return slot;
    }

    slot = new_global();
    update_symbol(slot, name, type, stype, C_GLOBAL, endlabel, size, 0);
    generate_global_symbol(slot);
    return slot;
}