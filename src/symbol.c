#include "../include/symbol.h"

static int new_global(void);


static int new_global(void) {
    int p;

    if ((p = num_globals++) > NUM_SYMBOLS) {
        fprintf(stderr, "Too many global symbols\n");
        exit(1);
    }

    return p;
}

int find_global(char* s) {
    int i = 0;

    for (; i < NUM_SYMBOLS; i++) {
        if (global_symbols[i].name != NULL && !strcmp(s, global_symbols[i].name)) {
            return i;
        }
    }

    return -1;
}

int add_global(char* name, int type, int stype, int endlabel, int size) {
    int y;

    if ((y = find_global(name)) != -1) {
        return y;
    }

    y = new_global();
    global_symbols[y].name = strdup(name);
    global_symbols[y].type = type;
    global_symbols[y].stype = stype;
    global_symbols[y].endlabel = endlabel;
    global_symbols[y].size = size;
    return y;
}