#include "../include/code_generation.h"

static int local_offset;
static int stack_offset;

/*
    Forward declarations
*/
static int allocate_register(void);
static void free_register(int indx);

static int cgcompare(int r1, int r2, char* how);

static int free_registers[NUM_FREE_REGISTERS];

static char *byte_register_list[] = { "%r10b", "%r11b", "%r12b", "%r13b", "%r9b", "%r8b", "%cl", "%dl", "%sil", "%rdi"};
static char *register_list[] = { "%r10", "%r11", "%r12", "%r13", "%r9", "%r8", "%rcx", "%rdx", "%rsi", "%rdi"};
static char *double_register_list[] = { "%r10d", "%r11d", "%r12d", "%r13d", "%r9d", "%r8d", "%ecx", "%edx", "%esi", "%edi" };


static char* cmplist[] = {"sete", "setne", "setl", "setg", "setle", "setge"};
static char* inv_cmplist[] = {"jne", "je", "jge", "jle", "jg", "jl"};

static int primitive_size[] 
    = {
        0,          // None
        0,          // Void
        1,          // Char
        4,          // Int
        8,          // Long
        8,          // char pointer
        8,          // int pointer
        8,          // long pointer
    };


static int isCompOperator(int tokenType) {
    switch(tokenType) {
        case A_EQUALS:
        case A_LESS_EQUAL:
        case A_GREATER_EQUAL:
        case A_LESS_THAN:
        case A_NOT_EQUAL:
        case A_GREATER_THAN:
            return 1;
        default:
            return 0;
    }
}

static int allocate_register(void) {
    for (int i = 0; i < 4; i++) {
        if (free_registers[i]) {
            free_registers[i] = 0;
            return i;
        }
    }

    fprintf(stderr, "No more registers available for allocation.\n");
    exit(1);
}

static int new_local_offset(int type) {
    local_offset += ((cgprimsize(type) > 4) ? cgprimsize(type) : 4);
    return -local_offset;
}

static int cgcompare(int r1, int r2, char* how) {
    fprintf(outfile, "\tcmpq\t%s, %s\n", register_list[r2], register_list[r1]);
    fprintf(outfile, "\t%s\t%s\n", how, byte_register_list[r2]);
    fprintf(outfile, "\tandq\t$255, %s\n", register_list[r2]);
    return r2;
}

void cglabel(int l) {
    fprintf(outfile, "L%d:\n", l);
}

void cgjump(int l) {
    fprintf(outfile, "\tjmp\tL%d\n", l);
}

void cgpreamble() {
    free_all_registers();
}

void cgpostamble() {}


int cgprimsize(int type) {

    // Pointer needs 8-byte on 64-bit
    if (pointer_type(type)) {return 8;}

    switch (type) {
        case TYPE_CHAR: return 1;
        case TYPE_INT: return 4;
        case TYPE_LONG: return 8;
        default:
            fprintf(stderr, "cgprimsize(): Bad type: %d\n", type);
            exit(1);
    }
    return 0;
}

enum { no_seg, text_seg, data_seg } currSeg = no_seg;

void cgtextseg() {
  if (currSeg != text_seg) {
    fputs("\t.text\n", outfile);
    currSeg = text_seg;
  }
}

void cgdataseg() {
  if (currSeg != data_seg) {
    fputs("\t.data\n", outfile);
    currSeg = data_seg;
  }
}

int cgequal(int r1, int r2) {return cgcompare(r1, r2, "sete");}
int cgnotequal(int r1, int r2) {return cgcompare(r1, r2, "setne");}
int cglessthan(int r1, int r2) {return cgcompare(r1, r2, "setl");}
int cggreaterthan(int r1, int r2) {return cgcompare(r1, r2, "setg");}
int cglessequal(int r1, int r2) {return cgcompare(r1, r2, "setle");}
int cggreaterequal(int r1, int r2) {return cgcompare(r1, r2, "setge");}


int cg_invert(int r1) {
    fprintf(outfile, "\tnotq\t%s\n", register_list[r1]);
    return r1;
}

int cg_negate(int r1) {
    fprintf(outfile, "\tnegq\t%s\n", register_list[r1]);
    return r1;
}


static void free_register(int indx) {
    if (free_registers[indx] != 0) {
        fprintf(stderr, "Error trying to free register %d\n", indx);
        exit(1);
    }

    free_registers[indx] = 1;
}

void free_all_registers(void) {
    free_registers[0] = free_registers[1]
        = free_registers[2] = free_registers[3] = 1;
}


int cgloadint(int value) {
    int r = allocate_register();
    fprintf(outfile, "\tmovq\t$%d, %s\n", value, register_list[r]);
    return r;
}

int cgloadglob(t_symbol_entry* symbol, int op) {
  int r = allocate_register();

  if (pointer_type(symbol->type)) {
      if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincq\t%s(\%%rip)\n", symbol->name);}
      if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecq\t%s(\%%rip)\n", symbol->name);}
      fprintf(outfile, "\tmovq\t%s(\%%rip), %s\n", symbol->name, register_list[r]);
      if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincq\t%s(\%%rip)\n", symbol->name);}
      if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecq\t%s(\%%rip)\n", symbol->name);}

  } else {
      // Print out the code to initialise it
      switch (symbol->type) {
        case TYPE_CHAR:
                if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincb\t%s(\%%rip)\n", symbol->name);}
                if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecb\t%s(\%%rip)\n", symbol->name);}

                fprintf(outfile, "\tmovzbq\t%s(%%rip), %s\n", symbol->name, register_list[r]);

                if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincb\t%s(\%%rip)\n", symbol->name);}
                if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecb\t%s(\%%rip)\n", symbol->name);}
            break;

        case TYPE_INT:
            if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincl\t%s(\%%rip)\n", symbol->name);}
            if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecl\t%s(\%%rip)\n", symbol->name);}
            fprintf(outfile, "\tmovslq\t%s(\%%rip), %s\n", symbol->name, register_list[r]);
            if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincl\t%s(\%%rip)\n", symbol->name);}
            if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecl\t%s(\%%rip)\n", symbol->name);}
            break;

        case TYPE_LONG:
            if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincq\t%s(\%%rip)\n", symbol->name);}
            if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecq\t%s(\%%rip)\n", symbol->name);}
            fprintf(outfile, "\tmovq\t%s(\%%rip), %s\n", symbol->name, register_list[r]);
            if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincq\t%s(\%%rip)\n", symbol->name);}
            if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecq\t%s(\%%rip)\n", symbol->name);}
            break;

        default:
            fprintf(stderr, "Bad type in cgloadglob: %d", symbol->type);
            exit(1);
        }
    }

    return r;
}

int cgstorderef(int r1, int r2, int type) {
    switch (type) {
        case TYPE_CHAR:
            fprintf(outfile, "\tmovb\t%s, (%s)\n", byte_register_list[r1], register_list[r2]);
            break;
        case TYPE_INT:
            fprintf(outfile, "\tmovq\t%s, (%s)\n", register_list[r1], register_list[r2]);
            break;
        case TYPE_LONG:
            fprintf(outfile, "\tmovq\t%s, (%s)\n", register_list[r1], register_list[r2]);
            break;
        default:
            fprintf(stderr, "Cant cgstoderef on type: %d\n", type);
    }
    
    return r1;
}

int cgstoreglob(int r, t_symbol_entry* symbol) {

    if (pointer_type(symbol->type)) {
        fprintf(outfile, "\tmovq\t%s, %s(\%%rip)\n", register_list[r], symbol->name);
    } else {
        switch (symbol->type) {
            case TYPE_CHAR:
                fprintf(outfile, "\tmovb\t%s, %s(\%%rip)\n", byte_register_list[r], symbol->name);
                break;
            case TYPE_INT:
                fprintf(outfile, "\tmovl\t%s, %s(\%%rip)\n", double_register_list[r], symbol->name);
                break;
            case TYPE_LONG:
                fprintf(outfile, "\tmovq\t%s, %s(\%%rip)\n", register_list[r], symbol->name);
                break;
            default:
                report_error("Bad type in cgloadglob: %d.\n", symbol->type);
                break;
        }
    }

    return r;
}

int cgloadlocal(t_symbol_entry* symbol, int op) {
    int r = allocate_register();

    if (pointer_type(symbol->type)) {
        if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincq\t%d(\%%rbp)\n", symbol->offset);}
        if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecq\t%d(\%%rbp)\n", symbol->offset);}
        fprintf(outfile, "\tmovq\t%d(\%%rbp), %s\n", symbol->offset, register_list[r]);
        if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincq\t%d(\%%rbp)\n", symbol->offset);}
        if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecq\t%d(\%%rbp)\n", symbol->offset);}
    } else {
        switch (symbol->type) {
            case TYPE_CHAR:
                if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincb\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecb\t%d(\%%rbp)\n", symbol->offset);}
                fprintf(outfile, "\tmovzbq\t%d(%%rbp), %s\n", symbol->offset,  register_list[r]);
                if (op == A_POST_INCREMENT) { fprintf(outfile, "\tincb\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecb\t%d(\%%rbp)\n", symbol->offset);}
                break;

            case TYPE_INT:
                if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincl\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecl\t%d(\%%rbp)\n", symbol->offset);}
                fprintf(outfile, "\tmovslq\t%d(\%%rbp), %s\n", symbol->offset, register_list[r]);
                if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincl\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecl\t%d(\%%rbp)\n", symbol->offset);}
                break;

            case TYPE_LONG:
                if (op == A_PRE_INCREMENT) {fprintf(outfile, "\tincq\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_PRE_DECREMENT) {fprintf(outfile, "\tdecq\t%d(\%%rbp)\n", symbol->offset);}
                fprintf(outfile, "\tmovq\t%d(\%%rbp), %s\n", symbol->offset, register_list[r]);
                if (op == A_POST_INCREMENT) {fprintf(outfile, "\tincq\t%d(\%%rbp)\n", symbol->offset);}
                if (op == A_POST_DECREMENT) {fprintf(outfile, "\tdecq\t%d(\%%rbp)\n", symbol->offset);}
                break;

            default:
                fprintf(stderr, "Bad type in cgloadglob: %d",  symbol->type);
                exit(1);
                break;
        }
    }

    return r;
}

int cgstorelocal(int r, t_symbol_entry* symbol) {
    if (pointer_type(symbol->type)) {
        fprintf(outfile, "\tmovq\t%s, %d(\%%rbp)\n", register_list[r], symbol->offset);
    } else {
        switch (symbol->type) {
            case TYPE_CHAR:
                fprintf(outfile, "\tmovb\t%s, %d(\%%rbp)\n", byte_register_list[r], symbol->offset);
                break;
            case TYPE_INT:
                fprintf(outfile, "\tmovl\t%s, %d(\%%rbp)\n", double_register_list[r], symbol->offset);
                break;
            case TYPE_LONG:
                fprintf(outfile, "\tmovq\t%s, %d(\%%rbp)\n", register_list[r], symbol->offset);
                break;
            default:
                fprintf(stderr, "Bad type in cgloadglob: %d.\n", symbol->type);
                break;
        }
    }

    return r;
}

void cgglobsym(t_symbol_entry* symbol) {
    int typesize;

    // Dont generate symbol for function
    if (symbol->stype == S_FUNCTION) {
        return;
    }

    typesize = cgprimsize(symbol->type);
    cgdataseg();

    fprintf(outfile, "\t.data\n" "\t.globl\t%s\n", symbol->name);
    fprintf(outfile, "%s:", symbol->name);

    for (int i = 0; i < symbol->size; i++) {
        switch(typesize) {
            case 1: fprintf(outfile, "\t.byte\t0\n"); break;
            case 4: fprintf(outfile, "\t.long\t0\n"); break;
            case 8: fprintf(outfile, "\t.quad\t0\n"); break;
            default: fprintf(stderr, "Unknown typesize in cgglobsym: %d\n", typesize); exit(1);
        }
    }
}

void cgreturn(int reg, t_symbol_entry* symbol) {
    switch (symbol->type) {
        case TYPE_CHAR:
            fprintf(outfile, "\tmovzbl\t%s, %%eax\n", byte_register_list[reg]);
            break;
        case TYPE_INT:
            fprintf(outfile, "\tmovl\t%s, %%eax\n", double_register_list[reg]);
            break;
        case TYPE_LONG:
            fprintf(outfile, "\tmovq\t%s, %%rax\n", register_list[reg]);
            break;
        default:
            report_error("Bad function type in cgreturn: %d.\n", symbol->type);
            break;
    }

    cgjump(symbol->endlabel);
}

int cgcall(t_symbol_entry* symbol, int argc) {
    // Get a new register
    int outr = allocate_register();

    fprintf(outfile, "\tcall\t%s\n", symbol->name);

    if (argc > 6) {
        fprintf(outfile, "\taddq\t$%d, %%rsp\n", 8*(argc-6));
    } else {
        fprintf(outfile, "\tmovq\t%%rax, %s\n", register_list[outr]);
    }

    return outr;
}

int cgxor(int r1, int r2) {
    fprintf(outfile, "\txorq\t%s, %s\n", register_list[r1], register_list[r2]);
    return r2; // Return register with result
}

void cgglobstr(int label, char* text) {
    cglabel(label);

    for (;*text != '\0'; text++) {
        fprintf(outfile, "\t.byte\t%d\n", *text);
    }

    fprintf(outfile, "\t.byte\t0\n");
}

int cgloadglobstr(int label) {
    int r = allocate_register();

    fprintf(outfile, "\tleaq\tL%d(%%rip), %s\n", label, register_list[r]);

    return r;
}

int cgwiden(int r, int oldtyxpe, int newtype) {
    return r;
}

int cgadd(int r1, int r2) {
    fprintf(outfile, "\taddq\t%s, %s\n", register_list[r1], register_list[r2]);

    free_register(r1);
    return r2;
}

int cgmul(int r1, int r2) {
    fprintf(outfile, "\timulq\t%s, %s\n", register_list[r1], register_list[r2]);

    free_register(r1);
    return r2;
}

int cgsub(int r1, int r2) {
    fprintf(outfile, "\tsubq\t%s, %s\n", register_list[r2], register_list[r1]);

    free_register(r2);
    return r1;
}

int cgshift_l(int r1, int r2) {
    fprintf(outfile, "\tmovb\t%s, %%cl\n", byte_register_list[r2]);
    fprintf(outfile, "\tshlq\t%%cl, %s\n", register_list[r1]);
    free_register(r2);
    return r1;
}

int cgshift_r(int r1, int r2) {
    fprintf(outfile, "\tmovb\t%s, %%cl\n", byte_register_list[r2]);
    fprintf(outfile, "\tshrq\t%%cl, %s\n", register_list[r1]);
    free_register(r2);
    return r1;
}

int cg_or(int r1, int r2) {
    fprintf(outfile, "\torq\t\t%s, %s\n", register_list[r1], register_list[r2]);
    free_register(r1);
    return r2;
}

int cg_and(int r1, int r2) {
    fprintf(outfile, "\tandq\t\t%s, %s\n", register_list[r1], register_list[r2]);
    free_register(r1);
    return r2;
}

int cg_logic_not(int r1) {
    fprintf(outfile, "\ttestq\t%s, %s\n", register_list[r1], register_list[r1]);
    fprintf(outfile, "\tsete\t%s\n", byte_register_list[r1]);
    fprintf(outfile, "\tmovzbq\t%s, %s\n", byte_register_list[r1], register_list[r1]);
    return r1;
}

int cgdiv(int r1, int r2) {

    fprintf(outfile, "\tmovq\t%s,%%rax\n", register_list[r1]);
    fprintf(outfile, "\tcqo\n");
    fprintf(outfile, "\tidivq\t%s\n", register_list[r2]);
    fprintf(outfile, "\tmovq\t%%rax,%s\n", register_list[r1]);
    free_register(r2);
    return r1;
}

void cgfunctionpreamble(t_symbol_entry* symbol) {
    char* name = symbol->name;
    t_symbol_entry* parameter, *local_var;

    int p_count;
    int param_offset = 16;
    int param_register = FIRST_PARAMETER_REGISTER;

    cgtextseg();
    local_offset = 0;

    fprintf(outfile,
        "\t.text\n"
        "\t.globl\t%s\n"
        "\t.type\t%s, @function\n"
        "%s:\n" "\tpushq\t%%rbp\n"
        "\tmovq\t%%rsp, %%rbp\n", name, name, name);

    // Copy in-register parameters onto stack
    for (parameter = symbol->member, p_count = 0; parameter != NULL; parameter = parameter->next, p_count++) {
        if (p_count > 6) {
            parameter->offset = param_offset;
            param_offset += 8;
        } else {
            parameter->offset = new_local_offset(parameter->type);
            cgstorelocal(param_register--, parameter);
        }
    }

    for (local_var = local_symbols->head; local_var != NULL; local_var = local_var->next) {
        local_var->offset = new_local_offset(local_var->type);
    }

    stack_offset = (local_offset + 15) & ~15;
    fprintf(outfile, "\taddq\t$%d, %%rsp\n",-stack_offset);
}

void cgfunctionpostamble(t_symbol_entry* symbol) {
    cglabel(symbol->endlabel);
    fprintf(outfile, "\taddq\t$%d,%%rsp\n", stack_offset);
    fputs("\tpopq %rbp\n" "\tret\n", outfile);
}

void generate_preamble() {cgpreamble();}
void generate_postamble() {cgpostamble();}
void generate_free_registers() {free_all_registers();}

int cgcompare_and_set(int ASTop, int r1, int r2) {
    if (!isCompOperator(ASTop)) {
        fprintf(stderr, "Bad ASTop in cgcompare_and_set()\n");
    }

    fprintf(outfile, "\tcmpq\t%s, %s\n", register_list[r2], register_list[r1]);
    fprintf(outfile, "\t%s\t%s\n", cmplist[ASTop - A_EQUALS], byte_register_list[r2]);
    fprintf(outfile, "\tmovzbq\t%s, %s\n", byte_register_list[r2], register_list[r2]);

    free_register(r1);
    return r2;
}

int cgcompare_and_jump(int ASTop, int r1, int r2, int label) {

    if (!isCompOperator(ASTop)) {
        fprintf(stderr, "Bad ASTop in cgcompare_and_jump()");
    }

    fprintf(outfile, "\tcmpq\t%s, %s\n", register_list[r2], register_list[r1]);
    fprintf(outfile, "\t%s\tL%d\n", inv_cmplist[ASTop - A_EQUALS], label);
    free_all_registers();
    return NOREG;
}

int cgaddress(t_symbol_entry* symbol) {
    int r = allocate_register();

    fprintf(outfile, "\tleaq\t%s(%%rip), %s\n", symbol->name, register_list[r]);
    return r;
}

int cgderef(int r, int type) {
    switch (value_at(type)) {
        case TYPE_CHAR:
            fprintf(outfile, "\tmovzbq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
        case TYPE_INT:
            fprintf(outfile, "\tmovslq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
        case TYPE_LONG:
            fprintf(outfile, "\tmovq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
    }
    return r;
}

int cgshlconst(int r, int val) {
    fprintf(outfile, "\tsalq\t$%d, %s\n", val, register_list[r]);
    return r;
}

void cg_reset_locals(void) {
    local_offset = 0;
}

int cg_get_local_offset(int type, int isparam) {
    local_offset += cgprimsize(type) > 4 ? cgprimsize(type) : 4;
    return -local_offset;
}

void cg_copy_argument(int r, int arg_position) {
    if (arg_position > 6) {
        fprintf(outfile, "\tpushq\t%s\n", register_list[r]);
    } else {
        fprintf(outfile, "\tmovq\t%s, %s\n", register_list[r],
            register_list[FIRST_PARAMETER_REGISTER - arg_position + 1]);
    }
}