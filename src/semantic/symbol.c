#include "../include/symbol.h"
#include "../../include/types.h"

// Creates a new symbol from the provided information
static t_symbol_entry* new_symbol(
        char* name,
        int type,
        t_symbol_entry* ctype,
        int stype,
        int class,
        int number_elements,
        int offset) {
    t_symbol_entry* entry = malloc(sizeof(t_symbol_entry));

    // Duplicate name in order to make it unique
    entry->name = strdup(name);
    entry->type = type;
    entry->stype = stype;
    entry->class = class;
    entry->ctype = ctype;

    if (pointer_type(type) || inttype(type)) {
        entry->size = number_elements * typesize(type, ctype);
    }

    entry->offset = offset;

    return entry;
}

void setup_symbol_table(void) {
    global_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    local_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    parameter_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    struct_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    member_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    union_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    enum_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
    typedef_symbols = (t_symbol_list *)malloc(sizeof(t_symbol_list));
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

t_symbol_entry* add_global_symbol(char* name,
                                  int type,
                                  t_symbol_entry* ctype,
                                  int stype,
                                  int class,
                                  int number_elements,
                                  int offset) {
    t_symbol_entry* entry = new_symbol(name, type, ctype, stype, class, number_elements, offset);
    add_symbol(global_symbols, entry);
    return entry;
}

t_symbol_entry* add_local_symbol(char* name,
                                 int type,
                                 t_symbol_entry* ctype,
                                 int stype,
                                 int number_elements) {
    t_symbol_entry* entry = new_symbol(name, type, ctype, stype, C_LOCAL, number_elements, 0);
    add_symbol(local_symbols, entry);

    if (type == TYPE_STRUCT || type == TYPE_UNION) {
        entry->size = ctype->size;
    }

    return entry;
}

t_symbol_entry* add_parameter_symbol(char* name,
                                     int type,
                                     t_symbol_entry* ctype,
                                     int stype) {
    t_symbol_entry* entry = new_symbol(name, type, ctype, stype, C_PARAMETER, 1, 0);
    add_symbol(parameter_symbols, entry);

    if (type == TYPE_STRUCT || type == TYPE_UNION) {
        entry->size = ctype->size;
    }

    return entry;
}

t_symbol_entry* add_struct_symbol(char* name) {
    t_symbol_entry* entry = new_symbol(name, TYPE_STRUCT, NULL, 0, C_STRUCT, 0, 0);
    add_symbol(struct_symbols, entry);
    return entry;
}

t_symbol_entry* add_union_symbol(char* name) {
    t_symbol_entry* entry = new_symbol(name, TYPE_STRUCT, NULL, 0, C_STRUCT, 0, 0);
    add_symbol(struct_symbols, entry);
    return entry;
}

t_symbol_entry* add_enum_symbol(char* name, int class, int value) {
    t_symbol_entry* entry = new_symbol(name, TYPE_INT, NULL, 0, class, 0, value);
    add_symbol(enum_symbols, entry);
    return entry;
}

t_symbol_entry* add_member_symbol(char* name,
                                  int type,
                                  t_symbol_entry* ctype,
                                  int stype,
                                  int number_elements) {
    t_symbol_entry* entry = new_symbol(name, type, ctype, stype, C_MEMBER, number_elements, 0);
    add_symbol(member_symbols, entry);
    return entry;
}


t_symbol_entry* add_typedef_symbol(char* name,
                                   int type,
                                   t_symbol_entry* ctype) {
    t_symbol_entry* entry = new_symbol(name, type, ctype, 0, C_TYPEDEF, 0, 0);
    add_symbol(typedef_symbols, entry);
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

void clear_local_symbol_table() {
    local_symbols->head = local_symbols->tail = NULL;
}

t_symbol_entry* find_enum_value(char* name) {
    t_symbol_entry *node = enum_symbols->head;
    while (node != NULL) {

        if (node->type == C_ENUM_VALUE && node->name != NULL && strcmp(node->name, name) == 0) {
            return node;
        }

        node = node->next;
    }

    return NULL;
}

t_symbol_entry* find_local_symbol(char* name) {
    t_symbol_entry* node;

    if (function_id) {
        node = find_symbol_in_list(parameter_symbols, name);

        if (node) {
            return node;
        }
    }

    return find_symbol_in_list(local_symbols, name);
}

t_symbol_entry* find_global_symbol(char* name) {
    return find_symbol_in_list(global_symbols, name);
}

t_symbol_entry* find_member_symbol(char* name) {
    return find_symbol_in_list(member_symbols, name);
}

static t_symbol_entry* find_parameter_symbol(t_symbol_entry* function, char* name) {
    while (function != NULL) {
        if (!strcmp(function->name, name)) {
            return function;
        }

        function = function->next;
    }

    return NULL;
}

t_symbol_entry* find_symbol(char* name) {
    t_symbol_entry* node;

    if (function_id) {
        node = find_parameter_symbol(function_id->member, name);

        if (node) {
            return node;
        }
    }

    node = find_local_symbol(name);
    if (node) {
        return node;
    }

    return find_global_symbol(name);
}

t_symbol_entry* find_struct_symbol(char* name) {
    return find_symbol_in_list(struct_symbols, name);
}

t_symbol_entry* find_union_symbol(char* name) {
    return find_symbol_in_list(union_symbols, name);
}

t_symbol_entry* find_enum_symbol(char* name) {
    return find_symbol_in_list(enum_symbols, name);
}

t_symbol_entry* find_typedef_symbol(char* name) {
    return find_symbol_in_list(typedef_symbols, name);
}

void clear_parameter_symbols(void) {
    parameter_symbols->head = parameter_symbols->tail = NULL;
}

void clear_symbol_table(void) {
    global_symbols->head = global_symbols->tail = NULL;
    local_symbols->head = local_symbols->tail = NULL;
    parameter_symbols->head = parameter_symbols->tail = NULL;
}