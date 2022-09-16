#include "../include/generation.h"

// Forward declarations
static int generate_if_AST(t_astnode* n, int loop_start_label, int loop_end_label);
static int generate_while_AST(t_astnode* n);
static int generate_switch_AST(t_astnode* n);

static int generate_function_call(t_astnode* n);


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
                 int parent_ast_op) {
    if (n == NULL) {
        return NOREG;
    }

    int leftreg, rightreg;

    switch (n->op) {
        case A_IF:
            return generate_if_AST(n, loop_start_label, loop_end_label);
        case A_WHILE:
            return generate_while_AST(n);
        case A_GLUE:
            generate_ast(n->left, if_label, loop_start_label, loop_end_label, n->op);
            generate_free_registers();
            generate_ast(n->right, if_label, loop_start_label, loop_end_label, n->op);
            generate_free_registers();
            return NOREG;
        case A_FUNCTION:
            cgfunctionpreamble(n->symbol);
            generate_ast(n->left, if_label, loop_start_label, loop_end_label, n->op);
            cgfunctionpostamble(n->symbol);
            return NOREG;
        case A_FUNCTION_CALL:
            return generate_function_call(n);
        case A_SWITCH:
            return generate_switch_AST(n);
    }

    if (n->left) {
        leftreg = generate_ast(n->left, if_label, loop_start_label, loop_end_label, n->op);
    }

    if (n->right) {
        rightreg = generate_ast(n->right, if_label, loop_start_label, loop_end_label, n->op);
    }

    switch (n->op) {
        case A_BREAK: cgjump(loop_end_label); return NOREG;
        case A_CONTINUE: cgjump(loop_start_label); return NOREG;
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
            if (parent_ast_op == A_IF || parent_ast_op == A_WHILE) {
                return cgcompare_and_jump(n->op, leftreg, rightreg, if_label);
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
        case A_POST_INCREMENT:
        case A_POST_DECREMENT:
                if (n->symbol->class == C_GLOBAL) {
                    return cgloadglob(n->symbol, n->op);
                } else {
                    return cgloadlocal(n->symbol, n->op);
                }
        case A_PRE_INCREMENT:
        case A_PRE_DECREMENT:
            if (n->symbol->class == C_GLOBAL) {
                return cgloadglob(n->left->symbol, n->op);
            } else {
                return cgloadlocal(n->left->symbol, n->op);
            }
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
        reg = generate_ast(gluetree->right, NOLABEL, NOLABEL, NOLABEL, gluetree->op);

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

static int generate_if_AST(t_astnode* n,
                           int loop_start_label,
                           int loop_end_label) {
    int lfalse, lend;

    // Two labels:
    //      one for false statement
    //      one for end of if statement
    lfalse = label();

    if (n->right) {
        lend = label();
    }

    generate_ast(n->left, lfalse, NOLABEL, NOLABEL, n->op);
    generate_free_registers();

    generate_ast(n->middle, NOLABEL, loop_start_label, loop_end_label, n->op);
    generate_free_registers();

    if (n->right) {
        cgjump(lend);
    }

    cglabel(lfalse);

    if (n->right) {
        generate_ast(n->right, NOLABEL, NOLABEL, NOLABEL, n->op);
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

    generate_ast(n->left, lend, lstart, lend, n->op);
    generate_free_registers();

    generate_ast(n->right, NOLABEL, lstart, lend, n->op);
    generate_free_registers();

    cgjump(lstart);
    cglabel(lend);

    return NOREG;
}

int generate_switch_AST(t_astnode* n) {
    int *case_value, *case_label;
    int label_jmp_top, label_end, label_default;

    int reg;
    int case_count = 0;

    case_value = (int*)malloc(sizeof(int) * (n->value + 1));
    case_label = (int*)malloc(sizeof(int) * (n->value + 1));

   label_jmp_top = label();
   label_end = label();

   label_default = label_end;

   reg = generate_ast(n->left, NOLABEL, NOLABEL, NOLABEL, 0);
   cgjump(label_jmp_top);
   generate_free_registers();

   int i;
   t_astnode * c;
   for (i = 0, c = n->right; c != NULL; i++, c = c->right) {
       case_label[i] = label();
       case_value[i] = c->value;

       cglabel(case_label[i]);

       if (c->op == A_DEFAULT) {
           label_default = case_label[i];
       } else {
           case_count++;
       }

       generate_ast(c->left, NOLABEL, NOLABEL, label_end, 0);
       generate_free_registers();
   }

   cgjump(label_end);

   cgswitch(reg, case_count, label_jmp_top, case_label, case_value, label_default);
   cglabel(label_end);

   return NOREG;
}


void generate_global_symbol(t_symbol_entry* symbol) {
    cgglobsym(symbol);
}