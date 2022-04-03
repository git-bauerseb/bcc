#include "../include/types.h"

int pointer_to(int type) {
    int p_type;

    switch(type) {
        case P_VOID: p_type = P_VOIDPTR; break;
        case P_CHAR: p_type = P_CHARPTR; break;
        case P_INT: p_type = P_INTPTR; break;
        case P_LONG: p_type = P_LONGPTR; break;
        default:
            fprintf(stderr, "Unrecognized in pointer_to(): %d\n", type);
    }


    return p_type;
}

int value_at(int type) {
    int n_type;

    switch (type) {
        case P_VOIDPTR: n_type = P_VOID; break;
        case P_CHARPTR: n_type = P_CHAR; break;
        case P_INTPTR: n_type = P_INT; break;
        case P_LONGPTR: n_type = P_LONG; break;
        default:
            fprintf(stderr, "Unrecognized in value_at(): %d\n", type);
    }

    return n_type;
}