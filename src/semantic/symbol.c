#include "../include/symbol.h"

// Creates a new symbol.
static t_symbol_entry* new_symbol(
        char* name,
        int type,
        int stype,
        t_symbol_entry* ctype,
        int size,
        int class,
        int offset
        ) {
    t_symbol_entry* entry = malloc(sizeof(t_symbol_entry));
    // Duplicate name in order to make it unique
    entry->name = strdup(name);
    entry->type = type;
    entry->stype = stype;
    entry->class = class;
    entry->ctype = ctype;
    entry->size = size;
    entry->offset = offset;

    if (class == C_GLOBAL) {
        generate_global_symbol(entry);
    }

    return entry;
}

void add_symbol(t_symbol_list* list, t_symbol_entry* s_entry) {
    if (list->tail != NULL) {
        list->tail->next = s_entry;
        list->tail = list->tail->next;
    } else {
        list->head = list->tail = s_entry;
    }

    s_entry->next = NULL;
}

void setup_symbol_table(void) {
    global_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    local_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    parameter_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    struct_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    member_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    union_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
}

t_symbol_entry* add_local_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, class, 0);
    add_symbol(local_symbols, entry);
    return entry;
}

t_symbol_entry* add_global_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, class, 0);
    add_symbol(global_symbols, entry);
    return entry;
}

t_symbol_entry* add_parameter_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, class, 0);
    add_symbol(parameter_symbols, entry);
    return entry;
}

t_symbol_entry* add_struct_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, 0, 0);
    add_symbol(struct_symbols, entry);
    return entry;
}

t_symbol_entry* add_union_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, 0, 0);
    add_symbol(union_symbols, entry);
    return entry;
}

t_symbol_entry* add_member_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size) {
    t_symbol_entry* entry = new_symbol(name, type, stype, ctype, size, 0, 0);
    add_symbol(member_symbols, entry);
    return entry;
}

t_symbol_entry* find_symbol_in_list(t_symbol_list* list, char* name) {
    t_symbol_entry *node = list->head;
    while (node != NULL) {

        if (node->name != NULL && strcmp(node->name, name) == 0) {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

t_symbol_entry* find_symbol_local(char* name) {
    t_symbol_entry* node;

    if (function_id) {
        node = find_symbol_in_list(parameter_symbols, name);

        if (node) {
            return node;
        }
    }

    return find_symbol_in_list(local_symbols, name);
}

t_symbol_entry* find_symbol_global(char* name) {
    return find_symbol_in_list(global_symbols, name);
}

t_symbol_entry* find_symbol_member(char* name) {
    return find_symbol_in_list(member_symbols, name);
}

t_symbol_entry* find_symbol(char* name) {
    t_symbol_entry* node;

    if (function_id) {
        node = find_symbol_in_list(parameter_symbols, name);

        if (node) {
            return node;
        }
    }

    node = find_symbol_local(name);
    if (node) {
        return node;
    }

    return find_symbol_global(name);
}

t_symbol_entry* find_struct_symbol(char* name) {
    return find_symbol_in_list(struct_symbols, name);
}

t_symbol_entry* find_union_symbol(char* name) {
    return find_symbol_in_list(union_symbols, name);
}

void clear_parameter_symbols(void) {
    parameter_symbols->head = parameter_symbols->tail = NULL;
}

void clear_symbol_table(void) {
    global_symbols->head = global_symbols->tail = NULL;
    local_symbols->head = local_symbols->tail = NULL;
    parameter_symbols->head = parameter_symbols->tail = NULL;
}