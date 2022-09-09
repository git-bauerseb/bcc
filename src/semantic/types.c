#include "../../include/types.h"

static const char* primitive_names[] = {
        "void",
        "char",
        "short",
        "int",
        "long"
};

// Returns true if the provided type is an integral value.
int inttype(int type) {
    return (type & 0xf) != 0;
}

// Returns true if the provided type is a pointer.
int pointer_type(int type) {
    return (type >> 4) != 0;
}

// Given a primitive type, returns the type which is a pointer to
// this type.
int pointer_to(int type) {
    if ((type & 0xf) == 0) {
        fprintf(stderr, "pointer_to(): Unrecognized primitive type %d\n", type);
        exit(1);
    }

    // Type is a primtive type then first convert it to a pointer
    return (type & 0xf) | (((type >> 4) + 1) << 4);
}

// Given a primitive pointer type, return the type it points to.
// int*** -> int**
int value_at(int type) {
    if ((type & 0xf) == 0) {
        fprintf(stderr, "value_at(): Unrecognized primitive type in pointer %d\n", type);
    }

    int shifted = type >> 4;

    if (shifted != 0) {
        return (type & 0xf) | (((type & 0xffffff0 >> 4) - 1) << 4);
    } else {
        return type & 0xf;
    }
}

char* get_type_representation(int type) {
    char* buffer = (char*)malloc(sizeof(char) * MAX_TYPE_LENGTH);
    int t_idx = (type & 0xf) - 1;

    if (t_idx > 5 || t_idx < 0) {
        fprintf(stderr, "Error, invalid type.\n");
    }

    int len = strlen(primitive_names[t_idx]);
    memcpy(buffer, primitive_names[t_idx], len);

    int i = 0;
    while (i < (type >> 4)) {
        buffer[len+i] = '*';
        i++;
    }

    buffer[len+i] = '\0';
    return buffer;
}

int typesize(int type, t_symbol_entry* ctype) {
    if (type == TYPE_STRUCT || type == TYPE_UNION) {
        return ctype->size;
    }

    return get_primitive_size(type);
}