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
            generate_ast(n->left, NOLABEL, n->op);
            generate_free_registers();
            generate_ast(n->right, NOLABEL, n->op);
            generate_free_registers();
            return NOREG;
        case A_FUNCTION:
            cgfunctionpreamble(n->v.id);
            generate_ast(n->left, NOLABEL, n->op);
            cgfunctionpostamble(n->v.id);
            return NOREG;
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
        case A_INTLIT: return cgloadint(n->v.value);
        case A_LSHIFT: return cgshift_l(leftreg, rightreg);
        case A_RSHIFT: return cgshift_r(leftreg, rightreg);
        case A_OR: return cg_or(leftreg, rightreg);
        case A_AND: return cg_and(leftreg, rightreg);
        case A_LOGIC_NOT: return cg_logic_not(leftreg);
        case A_IDENTIFIER:
            if (n->rvalue || parentASTop== A_DEREFERENCE) {
                return (cgloadglob(n->v.id, n->op));
            } else {
                return NOREG;
            }
        case A_LVIDENT: return cgstoreglob(reg, n->v.id);
        case A_ASSIGN: 
            switch (n->right->op) {
                case A_IDENTIFIER: return (cgstoreglob(leftreg, n->right->v.id));
                case A_DEREFERENCE: return (cgstorderef(leftreg, rightreg, n->right->type));
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
        case A_PRINT:
            generate_printint(leftreg);
            generate_free_registers();
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
            // If rvalue -> dereference, else leave for for assignment to store through pointer
            if (n->rvalue) {
                return cgderef(leftreg, n->left->type);
            } else {
                return leftreg;
            }
        case A_INVERT: return cg_invert(leftreg);
        case A_NEGATE: return cg_negate(leftreg);
        case A_PRE_INCREMENT:
                return cgloadglob(n->left->v.id, n->op);
        case A_PRE_DECREMENT:
                return cgloadglob(n->left->v.id, n->op);
        case A_POST_INCREMENT:
                return cgloadglob(n->v.id, n->op);
        case A_POST_DECREMENT:
                return cgloadglob(n->v.id, n->op);
        case A_SCALE:
            switch (n->v.size) {
                case 2: return cgshlconst(leftreg, 1);
                case 4: return cgshlconst(leftreg, 2);
                case 8: return cgshlconst(leftreg, 3);
                default:
                    rightreg = cgloadint(n->v.size);
                    return cgmul(leftreg, rightreg);
            }
        case A_STRLIT:
            return cgloadglobstr(n->v.id);
        case A_XOR:
            return cgxor(leftreg, rightreg);
        default:
            fprintf(stderr, "Unknown AST operator %d\n", n->op);
            exit(1);
    }

    return NOREG;
}

void generate_global_symbol(int id) {
    cgglobsym(id);
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