#ifndef GENERATE_H
#define GENERATE_H

#include "ast.h"
#include "cg.h"


/*
    Given AST, the register (if available) that holds
    the previous rvalue, and the AST op of the parent,
    generate assembly code.

    Return register index with the tree's final value.
*/
int generate_ast(t_astnode* n, int reg, int parentASTop);

void generate_global_symbol(int id);
int label(void);

#endif