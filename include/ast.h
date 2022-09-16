#ifndef PARSE_H
#define PARSE_H

#include <stdio.h>
#include <stdlib.h>

#include "scanner.h"
#include "code_generation.h"
#include "symbol.h"
#include "definitions.h"
#include "types.h"

// Ast generation functions
t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, t_symbol_entry* symbol, int value);
t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, t_symbol_entry* symbol, int value);
t_astnode* make_ast_leaf(int op, int type, t_symbol_entry* symbol, int value);
t_astnode* make_unary_ast_node(int op, int type, t_astnode *left, t_symbol_entry* symbol, int value);

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
t_astnode* modify_types(t_astnode* tree, int rtype, int op);

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
t_astnode* compound_statement(int is_switch_stmt);

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
t_symbol_entry* function_declaration(
        char* function_name,
        int type,
        t_symbol_entry* composite_type,
        int class
        );

// <var_declaration_list> ::= <epsilon>
//                           | <variable_declaration>
//                           | <variable_declaration> ',' <var_declaration_list>
int var_declaration_list(t_symbol_entry* function_symbol, int class, int separator, int end_token);

// Parses global variables or functions
void global_declarations(void);

// Parses declaration of a variable or an array
// <var_declaration> ::= <type> <identifier>
//                     | <type> <identifier> '[' <int> ']'
t_symbol_entry* var_declaration(int type, t_symbol_entry* ctype, int class);

// Types
int inttype(int type);
int parse_type(t_symbol_entry** ctpye, int* class);

// Generation
int generate_ast(t_astnode* n,
                 int if_label,
                 int loop_start_label,
                 int loop_end_label,
                 int parent_ast_op);
int generate_global_string(char* text);
int label(void);

// <expression_list> ::= <epsilon>
//                     | <expression>
//                     | <expression> ',' <expression_list>
t_astnode* expression_list(int end_token);

int declaration_list(
        t_symbol_entry** comp_type,
        int class,
        int end1,
        int end2
);

// Debugging
extern void print_ast(t_astnode* root, int depth);

extern t_token token;
extern int current_function_id;

// Depth of the current loop (while/for/do-while) we are currently in
// loop_level = 0 indicates we are in no loop.
int loop_level;

int switch_level;

#endif