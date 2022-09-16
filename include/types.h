#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <string.h>

#include "definitions.h"
#include "scanner.h"
#include "symbol.h"

#define MAX_TYPE_LENGTH 32

// Returns true if the provided type is an integral value.
int inttype(int type);

// Given a primitive type, returns the type which is a pointer to
// this type.
int pointer_to(int type);

// Returns true if the provided type is a pointer.
int pointer_type(int type);

// Given a primitive pointer type, return the type it points to.
// E.g. int*** -> int**.
int value_at(int type);

// Get a string representing the type provided by the argument
char* get_type_representation(int type);

// Depending whether the provided type is composite or primitive,
// returns the size of the type in bytes.
int typesize(int type, t_symbol_entry* comp_type);

int typesize(int type, t_symbol_entry* ctype);

extern int get_primitive_size(int type);

// Ast generation functions
extern t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, t_symbol_entry* symbol, int value);
extern t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, t_symbol_entry* symbol, int value);
extern t_astnode* make_ast_leaf(int op, int type, t_symbol_entry* symbol, int value);
extern t_astnode* make_unary_ast_node(int op, int type, t_astnode *left, t_symbol_entry* symbol, int value);

#endif