#include "../../include/ast.h"

t_astnode* compound_statement() {

    t_astnode* left = NULL;
    t_astnode* tree;

    match(T_LEFT_BRACE, "{");

    while (1) {
        tree = single_statement();

        if (tree != NULL 
            && (tree->op == A_ASSIGN
                || tree->op == A_FUNCTION_CALL || tree->op == A_RETURN)) {
            match(T_SEMICOLON, ";");
        }

        if (tree != NULL) {
            if (left == NULL) {
                left = tree;
            } else {
                left = make_astnode(A_GLUE, TYPE_NONE, left, tree, NULL, 0);
            }
        }

        if (token.token == T_RIGHT_BRACE) {
            match(T_RIGHT_BRACE, "}");
            return left;
        }
    }
}

static t_astnode* for_statement() {
    t_astnode* init, *cond, *inc, *body, *tree;

    match(T_FOR, "for");
    match(T_LEFT_PAREN, "(");

    init = single_statement();
    match(T_SEMICOLON, ";");

    cond = binary_expression();
    match(T_SEMICOLON, ";");

    inc = single_statement();
    match(T_RIGHT_PAREN, ")");

    body = compound_statement();

    tree = make_astnode(A_GLUE, TYPE_NONE, body, inc, NULL, 0);
    tree = make_astnode(A_WHILE, TYPE_NONE, cond, tree, NULL, 0);
    tree = make_astnode(A_GLUE, TYPE_NONE, init, tree, NULL, 0);

    return tree;
}

t_astnode* single_statement() {
    int type;
    switch(token.token) {
        case T_INT:
        case T_CHAR:
            type = parse_type();
            match(T_IDENTIFIER, "identifier");
            var_declaration(type, C_LOCAL);           // Parse local variables
            match(T_SEMICOLON, ";");
            return NULL;
        case T_IF:
            return if_statement();
        case T_WHILE:
            return while_statement();
        case T_FOR:
            return for_statement();
        case T_RETURN:
            return return_statement();
        default:
            return binary_expression();
            // fprintf(stderr, "Syntax error, token %d\n", token.token);
    }
}

t_astnode* return_statement(void) {
    t_astnode* tree;
    int returntype, function_type;

    if (function_id->type == TYPE_VOID) {
        fprintf(stderr, "Cannot return from a void function");
    }

    match(T_RETURN, "return");
    tree = binary_expression();
    tree = modify_type(tree, function_id->type, 0);

    if (tree == NULL) {
        fprintf(stderr, "Incompatible type to return.\n");
        exit(1);
    }

    tree = make_ast_unary(A_RETURN, TYPE_NONE, tree, NULL, 0);

    return tree;
}

/*
t_astnode* assignment_statement(void) {
    t_astnode* left, *right, *tree;
    int id;

    match(T_IDENTIFIER, "identifier");

    if (token.token == T_LEFT_PAREN) {
        return function_calls();
    }

    if ((id = find_symbol(text)) == -1) {
        fprintf(stderr, "Undeclared variable %s\n", text);
        exit(1);
    }

    // Not reject token because either '(' or '=' are coming
    // after identifier

    right = make_ast_leaf(A_LVIDENT, sym_table[id].type, id);
    match(T_ASSIGNMENT, "=");

    left = binary_expression();

    left = modify_type(left, right->type, 0);

    if (left == NULL) {
        fprintf(stderr, "Incompatible expression in assignment.\n");
        exit(1);
    }

    tree = make_astnode(A_ASSIGN, TYPE_NONE, left, right, 0);

    return tree;
}
*/

t_astnode* if_statement(void) {
    t_astnode* condAST, *trueAST, *falseAST = NULL;

    match(T_IF, "if");
    match(T_LEFT_PAREN, "(");

    condAST = binary_expression();

    if (condAST->op < A_EQUALS || condAST->op > A_GREATER_EQUAL) {
        fprintf(stderr, "Bad comparison operator.\n");
    }

    match(T_RIGHT_PAREN, ")");

    trueAST = compound_statement();

    // If there is an additional 'else' then skip this token
    // and parse conditional part
    if (token.token == T_ELSE) {
        scan(&token);
        falseAST = compound_statement();
    }

    return make_ternary_astnode(A_IF, TYPE_NONE, condAST, trueAST, falseAST, NULL, 0);
}

t_astnode* while_statement(void) {
    t_astnode* condAST, *bodyAST;

    match(T_WHILE, "while");
    match(T_LEFT_PAREN, "(");

    condAST = binary_expression();

    if (condAST->op < A_EQUALS || condAST->op > A_GREATER_EQUAL) {
        fprintf(stderr, "Bad comparison operator\n");
    }

    match(T_RIGHT_PAREN, ")");

    bodyAST = compound_statement();

    return make_astnode(A_WHILE, TYPE_NONE, condAST, bodyAST, NULL, 0);
}