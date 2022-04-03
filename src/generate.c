#include "../include/generate.h"

/*
    Forward declarations.
*/
static int generate_if_AST(t_astnode* n);
static int generate_while_AST(t_astnode* n);


/*
    Given AST, the register (if available) that holds
    the previous rvalue, and the AST op of the parent,
    generate assembly code.

    Return register index with the tree's final value.
*/
int generate_ast(t_astnode* n, int reg, int parentASTop) {
    int leftreg, rightreg;

    switch (n->op) {
        case A_IF:
            return generate_if_AST(n);
        case A_WHILE:
            return generate_while_AST(n);
        case A_GLUE:
            generate_ast(n->left, NOREG, n->op);
            generate_free_registers();
            generate_ast(n->right, NOREG, n->op);
            generate_free_registers();
            return NOREG;
    }

    if (n->left) {
        leftreg = generate_ast(n->left, NOREG, n->op);
    }

    if (n->right) {
        rightreg = generate_ast(n->right, leftreg, n->op);
    }

    switch (n->op) {
        case A_ADD: return cgadd(leftreg, rightreg);
        case A_SUBTRACT: return cgsub(leftreg, rightreg);
        case A_MULTIPLY: return cgmul(leftreg, rightreg);
        case A_DIVIDE: return cgdiv(leftreg, rightreg);
        case A_INTLIT: return cgloadint(n->v.value);
        case A_IDENTIFIER: return cgloadglob(n->v.id);
        case A_LVIDENT: return cgstoreglob(reg, n->v.id);
        case A_ASSIGN: return rightreg;
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
        case A_PRINT:
            generate_printint(leftreg);
            generate_free_registers();
            return NOREG;
        case A_FUNCTION:

            cgfunctionpreamble(n->v.id);
            generate_ast(n->left, NOREG, n->op);
            cgfunctionpostamble(n->v.id);
            return NOREG;
        case A_WIDEN:
            // Widen children type to parent type
            return cgwiden(leftreg, n->left->type, n->type);
        case A_RETURN:
            cgreturn(leftreg, current_function_id);
            return NOREG;
        case A_FUNCTION_CALL:
            return cgcall(leftreg, n->v.id);
        case A_ADDR:
            return cgaddress(n->v.id);
        case A_DEREFERENCE:
            return cgderef(leftreg, n->left->type);
        case A_SCALE:
            switch (n->v.size) {
                case 2: return cgshlconst(leftreg, 1);
                case 4: return cgshlconst(leftreg, 2);
                case 8: return cgshlconst(leftreg, 3);
                default:
                    rightreg = cgloadint(n->v.size);
                    return cgmul(leftreg, rightreg);
            }
        default:
            fprintf(stderr, "Unknown AST operator %d\n", n->op);
            exit(1);
    }
}

void generate_global_symbol(int id) {
    cgglobsym(id);
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

    generate_ast(n->middle, NOREG, n->op);
    generate_free_registers();

    if (n->right) {
        cgjump(lend);
    }

    cglabel(lfalse);

    if (n->right) {
        generate_ast(n->right, NOREG, n->op);
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

    generate_ast(n->right, NOREG, n->op);
    generate_free_registers();

    cgjump(lstart);
    cglabel(lend);

    return NOREG;
}