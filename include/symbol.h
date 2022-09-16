#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "error.h"

#define NUM_SYMBOLS 1024

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
t_symbol_list *enum_symbols;
t_symbol_list *typedef_symbols;

// FUNCTIONS
// Add a symbol to a symbol list
void add_symbol(t_symbol_list* list, t_symbol_entry* s_entry);

// (char *name, int type, struct symtable *ctype, int stype, int class, int nelems, int posn);

// Add a symbol to the global symbol list
t_symbol_entry* add_global_symbol(char* name,
                                  int type,
                                  t_symbol_entry* ctype,
                                  int stype,
                                  int class,
                                  int number_elements,
                                  int offset);

// Add a symbol to the local symbol list
t_symbol_entry* add_local_symbol(char* name,
                                 int type,
                                 t_symbol_entry* ctype,
                                 int stype,
                                 int number_elements);

// Add a symbol to the parameter symbol list
t_symbol_entry* add_parameter_symbol(char* name,
                                     int type,
                                     t_symbol_entry* ctype,
                                     int stype);

// Add a symbol to the struct symbol
t_symbol_entry* add_struct_symbol(char* name);
t_symbol_entry* add_union_symbol(char* name);

t_symbol_entry* add_enum_symbol(char* name, int class, int value);

// Add a symbol to the member symbol list
t_symbol_entry* add_member_symbol(char* name,
                                  int type,
                                  t_symbol_entry* ctype,
                                  int stype,
                                  int number_elements);

t_symbol_entry* add_typedef_symbol(char* name,
                                   int type,
                                   t_symbol_entry* ctype);


// Find a symbol (by name) in a given list. Returns true if the symbol is present
// and false if not.
t_symbol_entry* find_symbol_in_list(t_symbol_list* list, char* name);
t_symbol_entry* find_symbol(char* name);
t_symbol_entry* find_local_symbol(char* name);
t_symbol_entry* find_global_symbol(char* name);
t_symbol_entry* find_struct_symbol(char* name);
t_symbol_entry* find_union_symbol(char* name);
t_symbol_entry* find_member_symbol(char* name);
t_symbol_entry* find_enum_symbol(char* name);
t_symbol_entry* find_enum_value(char* name);
t_symbol_entry* find_typedef_symbol(char* name);


// Sets parameter list to NULL
void clear_parameter_symbols(void);

// Clear all symbol linked-lists
void clear_symbol_table(void);

// Remove all local symbols from the corresponding list.
void clear_local_symbol_table(void);

void copy_function_parameters(int id);

// Initializes the different lists for the symbols
void setup_symbol_table(void);

// EXTERN
extern void generate_global_symbol(t_symbol_entry* symbol);

// Symbol entry indicating the current function that is parsed.
// Needed in order to avoid for example duplicate definitions of variables.
extern t_symbol_entry* function_id;

#endif