#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "error.h"

#define NUM_SYMBOLS 1024

// Structural type of a symbol
enum {
    S_VARIABLE,
    S_FUNCTION,
    S_ARRAY
};

// Storage class for a symbol
enum {
    C_GLOBAL = 1,       // Globally visible
    C_LOCAL,            // Locally visible
    C_PARAMETER,        // Locally visible function parameter
    C_MEMBER            // Member of a structure
};

// Entry in the symbol table. Stores all relevant informations
// for function types/structs/unions/enums declared in the program.
typedef struct symbol_table {
    char* name;                     // Name of a symbol
    int type;                       // Primitive type for symbol
                                    // Not used if the symbol is of composite type

    struct symbol_table* ctype;     // Type for a symbol that refers to a composite type.
    int stype;                      // Structural type for symbol
    int class;                      // Storage class for the symbol

    union {
        int size;                   // Number of elements in the symbol
        int endlabel;               // End label (the label to jump to) after function
    };

    union {
        int params;                 // Number of parameters for a function
        int offset;                 // Negative offset of base pointer for local variables
    };

    struct symbol_table *next;      // Next symbol in list
    struct symbol_table *member;    // Parameter of function, struct, union, enum, ...

} t_symbol_entry;

typedef struct symbol_list {
    t_symbol_entry *head;
    t_symbol_entry *tail;
} t_symbol_list;

// Linked-list of symbols for global variables and functions
t_symbol_list *global_symbols;

// Linked-list of symbols for local variables
t_symbol_list *local_symbols;

// Linked-list of symbols for local variables
t_symbol_list *parameter_symbols;

// Linked-list of symbols for struct types that were defined
t_symbol_list *struct_symbols;

// Linked-list of symbols for struct members
t_symbol_list *member_symbols;

t_symbol_list *union_symbols;


// FUNCTIONS
// Add a symbol to a symbol list
void add_symbol(t_symbol_list* list, t_symbol_entry* s_entry);

// Add a symbol to the global symbol list
t_symbol_entry* add_global_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class);

// Add a symbol to the local symbol list
t_symbol_entry* add_local_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class);

// Add a symbol to the parameter symbol list
t_symbol_entry* add_parameter_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size, int class);

// Add a symbol to the struct symbol
t_symbol_entry* add_struct_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size);

t_symbol_entry* add_union_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size);


// Add a symbol to the member symbol list
t_symbol_entry* add_member_symbol(char* name, int type, int stype, t_symbol_entry* ctype, int size);


// Find a symbol (by name) in a given list. Returns true if the symbol is present
// and false if not.
t_symbol_entry* find_symbol_in_list(t_symbol_list* list, char* name);

// Find a symbol that is either a local variable or a parameter
t_symbol_entry* find_symbol_local(char* name);

// Find a symbol that is global
t_symbol_entry* find_symbol_global(char* name);

t_symbol_entry* find_struct_symbol(char* name);

t_symbol_entry* find_union_symbol(char* name);

t_symbol_entry* find_symbol_member(char* name);

// Find symbol in any of the available symbol lists
t_symbol_entry* find_symbol(char* name);

// Sets parameter list to NULL
void clear_parameter_symbols(void);

// Clear all symbol linked-lists
void clear_symbol_table(void);

void copy_function_parameters(int id);

// Initializes the different lists for the symbols
void setup_symbol_table(void);

// EXTERN
extern void generate_global_symbol(t_symbol_entry* symbol);

// Symbol entry indicating the current function that is parsed.
// Needed in order to avoid for example duplicate definitions of variables.
extern t_symbol_entry* function_id;

#endif