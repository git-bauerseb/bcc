#include "../include/interpret.h"

int interpret_ast(t_astnode* n) {
    int left_val, right_val;

    if (n->left) {
        left_val = interpret_ast(n->left); 
    }

    if (n->right) {
        right_val = interpret_ast(n->right);
    }

    switch(n->op) {
        case A_ADD:
            return left_val + right_val;
        case A_SUBTRACT:
            return left_val - right_val;
        case A_MULTIPLY:
            return left_val * right_val;
        case A_DIVIDE:
            return left_val / right_val;
        case A_INTLIT:
            return n->v.value;
        default:
            fprintf(stderr, "Unknown AST operator %d\n", n->op);
            exit(1);
    }
}