#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "cg.h"
#include "symbol.h"
#include "definitions.h"
#include "types.h"

typedef struct astnode {
    int op;
    int type;
    int rvalue;                 // True if the node is an rvalue
    struct astnode* left;
    struct astnode* middle;
    struct astnode* right;
    union {
        int value;              // For A_INTLIT, the integer value
        int id;                 // For A_IDENT, the symbol slot number
        int size;               // For A_SCALE, the size to scale by
    } v;
} t_astnode;

// Ast generation functions
t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, int value);
t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, int value);
t_astnode* make_ast_leaf(int op, int type, int value);
t_astnode* make_ast_unary(int op, int type, t_astnode *left, int value);

void global_declarations(void);
int arithop(int tok);
void match(int t, char* to_match);
void generate_global_symbol(int id);


/*
    Given an AST tree and a type which we want it to become,
    possibly modify the tree by widening or scaling so that
    it is compatible with this type.

    Return the original tree if no changes occurred, a modified tree,
    or NULL if the tree is not compatible with the given type.

    If the tree is part of a binary operation, the AST op is not zero.
*/
t_astnode* modify_type(t_astnode* tree, int rtype, int op);


extern int generate_ast(t_astnode* n, int reg, int parentASTop);
extern int generate_global_string(char* text);
extern int label(void);

extern t_token token;
extern int current_function_id;
#endif