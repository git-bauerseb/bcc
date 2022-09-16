#ifndef GENERATE_H
#define GENERATE_H

#include "ast.h"
#include "code_generation.h"


/*
    Given AST, the register (if available) that holds
    the previous rvalue, and the AST op of the parent,
    generate assembly code.

    Return register index with the tree's final value.
*/
int generate_ast(t_astnode* n,
                 int if_label,
                 int loop_start_label,
                 int loop_end_label,
                 int parent_ast_op);

int generate_global_string(char* text);
void generate_global_symbol(t_symbol_entry* symbol);

int label(void);

#endif