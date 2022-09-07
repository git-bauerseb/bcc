#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>

#include "scan.h"
#include "code_generation.h"
#include "symbol.h"
#include "definitions.h"
#include "types.h"

// Sructure used in the Abstract-Systax Tree (AST).
typedef struct astnode {
    int op;
    int type;
    int rvalue;
    struct astnode* left;
    struct astnode* middle;
    struct astnode* right;
    t_symbol_entry* symbol;
    union {
        int value;              // For A_INTLIT, the integer value
        int size;               // For A_SCALE, the size to scale by
    };
} t_astnode;

// Ast generation functions
t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, t_symbol_entry* symbol, int value);
t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, t_symbol_entry* symbol, int value);
t_astnode* make_ast_leaf(int op, int type, t_symbol_entry* symbol, int value);
t_astnode* make_ast_unary(int op, int type, t_astnode *left, t_symbol_entry* symbol, int value);

void global_declarations(void);
int arithop(int tok);
void match(int t, char* to_match);


/*
    Given an AST tree and a type which we want it to become,
    possibly modify the tree by widening or scaling so that
    it is compatible with this type.

    Return the original tree if no changes occurred, a modified tree,
    or NULL if the tree is not compatible with the given type.

    If the tree is part of a binary operation, the AST op is not zero.
*/
t_astnode* modify_type(t_astnode* tree, int rtype, int op);

/*
    Externally defined
*/

// Expressions
t_astnode* binary_expression(void);
t_astnode* function_calls(void);

// Statements

// <compound_statement> ::= <epsilon>
//                        | <statement>
//                        | <statement> <statements>
t_astnode* compound_statement(void);

// <return> ::= 'return'
//            | 'return' <expression>
t_astnode* return_statement(void);

t_astnode* assignment_statement(void);

t_astnode* while_statement(void);
t_astnode* single_statement(void);
t_astnode* if_statement(void);

// Declarations

// <function_declaration> ::= <type> <identifier> '(' <parameter_list> ')'
//                          | <type> <identifier> '(' <parameter_list> ')' <compound_statement>
t_astnode* function_declaration(int type);

// <parameter_declaration> ::= <epsilon>
//                           | <variable_declaration>
//                           | <variable_declaration> ',' <parameter_declaration>
int parameter_declaration(t_symbol_entry* function_symbol);

// Parses global variables or functions
void global_declarations(void);

// Parses declaration of a variable or an array
// <var_declaration> ::= <type> <identifier>
//                     | <type> <identifier> '[' <int> ']'
t_symbol_entry* var_declaration(int type, int class);

// Types
int inttype(int type);
int parse_type(void);

// Generation
int generate_ast(t_astnode* n, int reg, int parentASTop);
int generate_global_string(char* text);
int label(void);

// Debugging
extern void print_ast(t_astnode* root, int depth);

extern t_token token;
extern int current_function_id;

#endif