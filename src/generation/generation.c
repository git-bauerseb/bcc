#include "../include/generation.h"

/*
    Forward declarations.
*/
static int generate_if_AST(t_astnode* n);
static int generate_while_AST(t_astnode* n);

static int generate_function_call(t_astnode* n);


/*
    Given AST, the register (if available) that holds
    the previous rvalue, and the AST op of the parent,
    generate assembly code.

    Return register index with the tree's final value.
*/
int generate_ast(t_astnode* n, int reg, int parentASTop) {
    if (n == NULL) {
        return NOREG;
    }

    int leftreg, rightreg;

    switch (n->op) {
        case A_IF:
            return generate_if_AST(n);
        case A_WHILE:
            return generate_while_AST(n);
        case A_GLUE:
            generate_ast(n->left, NOLABEL, n->op);
            generate_free_registers();
            generate_ast(n->right, NOLABEL, n->op);
            generate_free_registers();
            return NOREG;
        case A_FUNCTION:
            cgfunctionpreamble(n->symbol);
            generate_ast(n->left, NOLABEL, n->op);
            cgfunctionpostamble(n->symbol);
            return NOREG;
        case A_FUNCTION_CALL:
            return generate_function_call(n);
    }

    if (n->left) {
        leftreg = generate_ast(n->left, NOLABEL, n->op);
    }

    if (n->right) {
        rightreg = generate_ast(n->right, NOLABEL, n->op);
    }

    switch (n->op) {
        case A_ADD: return cgadd(leftreg, rightreg);
        case A_SUBTRACT: return cgsub(leftreg, rightreg);
        case A_MULTIPLY: return cgmul(leftreg, rightreg);
        case A_DIVIDE: return cgdiv(leftreg, rightreg);
        case A_INTLIT: return cgloadint(n->value);
        case A_LSHIFT: return cgshift_l(leftreg, rightreg);
        case A_RSHIFT: return cgshift_r(leftreg, rightreg);
        case A_OR: return cg_or(leftreg, rightreg);
        case A_AND: return cg_and(leftreg, rightreg);
        case A_LOGIC_NOT: return cg_logic_not(leftreg);
        case A_IDENTIFIER:
            if (n->symbol->class == C_LOCAL || n->symbol->class == C_PARAMETER) {
                return cgloadlocal(n->symbol, n->op);
            } else {
                return cgloadglob(n->symbol, n->op);
            }
        case A_ASSIGN: 
            switch (n->right->op) {
                case A_IDENTIFIER: 
                    if (n->right->symbol->class == C_LOCAL) {
                        return cgstorelocal(leftreg, n->right->symbol);
                    } else {
                        return cgstoreglob(leftreg, n->right->symbol);
                    }
                case A_DEREFERENCE:
                    return (cgstorderef(leftreg, rightreg, n->right->type));
                default: fprintf(stderr, "Cant assign in generate_ast(), op: %d\n", n->op);
            }
        case A_EQUALS:
        case A_NOT_EQUAL:
        case A_LESS_THAN:
        case A_GREATER_THAN:
        case A_LESS_EQUAL:
        case A_GREATER_EQUAL:
            if (parentASTop == A_IF || parentASTop == A_WHILE) {
                return cgcompare_and_jump(n->op, leftreg, rightreg, reg);
            } else {
                return cgcompare_and_set(n->op, leftreg, rightreg);
            }
        case A_WIDEN:
            // Widen children type to parent type
            return cgwiden(leftreg, n->left->type, n->type);
        case A_RETURN:
            cgreturn(leftreg, function_id);
            return NOREG;
        case A_FUNCTION_CALL:
            return generate_function_call(n);
        case A_ADDR:
            return cgaddress(n->symbol);
        case A_DEREFERENCE:
            // If rvalue -> dereference, else leave for for assignment to store through pointer
            if (n->rvalue) {
                return cgderef(leftreg, n->left->type);
            } else {
                return leftreg;
            }
        case A_INVERT: return cg_invert(leftreg);
        case A_NEGATE: return cg_negate(leftreg);
        case A_PRE_INCREMENT:
                return cgloadglob(n->left->symbol, n->op);
        case A_PRE_DECREMENT:
                return cgloadglob(n->left->symbol, n->op);
        case A_POST_INCREMENT:
                return cgloadglob(n->symbol, n->op);
        case A_POST_DECREMENT:
                return cgloadglob(n->symbol, n->op);
        case A_SCALE:
            switch (n->size) {
                case 2: return cgshlconst(leftreg, 1);
                case 4: return cgshlconst(leftreg, 2);
                case 8: return cgshlconst(leftreg, 3);
                default:
                    rightreg = cgloadint(n->size);
                    return cgmul(leftreg, rightreg);
            }
        case A_STRLIT:
            return cgloadglobstr(n->value);
        case A_XOR:
            return cgxor(leftreg, rightreg);
        default:
            fprintf(stderr, "Unknown AST operator %d\n", n->op);
            exit(1);
    }

    return NOREG;
}

static int generate_function_call(t_astnode* n) {
    t_astnode* gluetree = n->left;
    int reg;
    int args = 0;

    while (gluetree) {
        reg = generate_ast(gluetree->right, NOLABEL, gluetree->op);

        // Copy into nth function parameter
        cg_copy_argument(reg, gluetree->size);

        // Keep number of arguments
        args = (args == 0) ? gluetree->size : args;

        generate_free_registers();
        gluetree = gluetree->left;
    }

    return cgcall(n->symbol, args);
}

int generate_global_string(char* text) {
    int l = label();

    cgglobstr(l, text);

    return l;
}

int label(void) {
    static int id = 1;
    return id++;
}

static int generate_if_AST(t_astnode* n) {
    int lfalse, lend;

    // Two labels:
    //      one for false statement
    //      one for end of if statement
    lfalse = label();

    if (n->right) {
        lend = label();
    }

    generate_ast(n->left, lfalse, n->op);
    generate_free_registers();

    generate_ast(n->middle, NOLABEL, n->op);
    generate_free_registers();

    if (n->right) {
        cgjump(lend);
    }

    cglabel(lfalse);

    if (n->right) {
        generate_ast(n->right, NOLABEL, n->op);
        generate_free_registers();
        cglabel(lend);
    }

    return NOREG;
}

static int generate_while_AST(t_astnode* n) {
    int lstart, lend;

    lstart = label();
    lend = label();
    cglabel(lstart);

    generate_ast(n->left, lend, n->op);
    generate_free_registers();

    generate_ast(n->right, NOLABEL, n->op);
    generate_free_registers();

    cgjump(lstart);
    cglabel(lend);

    return NOREG;
}


void generate_global_symbol(t_symbol_entry* symbol) {
    cgglobsym(symbol);
}