#include "../include/cg.h"

/*
    Forward declarations
*/
static int allocate_register(void);
static void free_register(int indx);

static int cgcompare(int r1, int r2, char* how);

static int free_registers[4];

static char *byte_register_list[4] = { "%r8b", "%r9b", "%r10b", "%r11b" };
static char *register_list[4] = { "%r8", "%r9", "%r10", "%r11" };
static char *double_register_list[4] = { "%r8d", "%r9d", "%r10d", "%r11d" };


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


static int cgcompare(int r1, int r2, char* how) {
    fprintf(outfile, "\tcmpq\t%s, %s\n", register_list[r2], register_list[r1]);
    fprintf(outfile, "\t%s\t%s\n", how, byte_register_list[r2]);
    fprintf(outfile, "\tandq\t$255, %s\n", register_list[r2]);
    return r2;
}

int cgprimsize(int type) {
    if (type < P_NONE || type > P_LONGPTR) {
        fprintf(stderr, "Bad type in cgprimsize()\n");
    }

    return primitive_size[type];
}

int cgequal(int r1, int r2) {return cgcompare(r1, r2, "sete");}
int cgnotequal(int r1, int r2) {return cgcompare(r1, r2, "setne");}
int cglessthan(int r1, int r2) {return cgcompare(r1, r2, "setl");}
int cggreaterthan(int r1, int r2) {return cgcompare(r1, r2, "setg");}
int cglessequal(int r1, int r2) {return cgcompare(r1, r2, "setle");}
int cggreaterequal(int r1, int r2) {return cgcompare(r1, r2, "setge");}


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

int cgloadglob(int id) {
    int r = allocate_register();

    switch (global_symbols[id].type) {
        case P_CHAR:
            fprintf(outfile, "\tmovzbq\t%s(\%%rip), %s\n", global_symbols[id].name,
            register_list[r]);
            break;
        case P_INT:
            fprintf(outfile, "\tmovzbl\t%s(\%%rip), %s\n", global_symbols[id].name,
                register_list[r]);
            break;
        case P_LONG:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
            fprintf(outfile, "\tmovq\t%s(\%%rip), %s\n", global_symbols[id].name, register_list[r]);
            break;
        default:
            fprintf(stderr, "Bad type in cgloadglob: %d\n", global_symbols[id].type);
            break;
    }

    return r;
}

int cgstoreglob(int r, int id) {
    switch (global_symbols[id].type) {
        case P_CHAR:
            fprintf(outfile, "\tmovb\t%s, %s(\%%rip)\n", byte_register_list[r],
                    global_symbols[id].name);
            break;
        case P_INT:
            fprintf(outfile, "\tmovl\t%s, %s(\%%rip)\n", double_register_list[r],
                global_symbols[id].name);
            break;
        case P_LONG:
        case P_CHARPTR:
        case P_INTPTR:
        case P_LONGPTR:
            fprintf(outfile, "\tmovq\t%s, %s(\%%rip)\n", register_list[r], global_symbols[id].name);
            break;
        default:
        fprintf(stderr, "Bad type in cgloadglob: %d.\n", global_symbols[id].type);
    }
  return (r);
}

void cgreturn(int reg, int id) {
    // Generate code depending on the function's type
    switch (global_symbols[id].type) {
        case P_CHAR:
            fprintf(outfile, "\tmovzbl\t%s, %%eax\n", byte_register_list[reg]);
            break;
        case P_INT:
            fprintf(outfile, "\tmovl\t%s, %%eax\n", double_register_list[reg]);
            break;
        case P_LONG:
            fprintf(outfile, "\tmovq\t%s, %%rax\n", register_list[reg]);
            break;
        default:
                fprintf(stderr, "Bad function type in cgreturn: %d.\n", global_symbols[id].type);
    }
    cgjump(global_symbols[id].endlabel);
}

int cgcall(int r, int id) {
  // Get a new register
  int outr = allocate_register();
  fprintf(outfile, "\tmovq\t%s, %%rdi\n", register_list[r]);
  fprintf(outfile, "\tcall\t%s\n", global_symbols[id].name);
  fprintf(outfile, "\tmovq\t%%rax, %s\n", register_list[outr]);
  free_register(r);
  return (outr);
}

void cgglobsym(int id) {
    int typesize;
    typesize = cgprimsize(global_symbols[id].type);

    fprintf(outfile, "\t.data\n" "\t.globl\t%s\n", global_symbols[id].name);
    switch(typesize) {
        case 1: fprintf(outfile, "%s:\t.byte\t0\n", global_symbols[id].name); break;
        case 4: fprintf(outfile, "%s:\t.long\t0\n", global_symbols[id].name); break;
        case 8: fprintf(outfile, "%s:\t.quad\t0\n", global_symbols[id].name); break;
        default: fprintf(stderr, "Unknown typesize in cgglobsym: %d\n", typesize); exit(1);
  }
}

int cgwiden(int r, int oldtype, int newtype) {
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

int cgdiv(int r1, int r2) {

    fprintf(outfile, "\tmovq\t%s,%%rax\n", register_list[r1]);
    fprintf(outfile, "\tcqo\n");
    fprintf(outfile, "\tidivq\t%s\n", register_list[r2]);
    fprintf(outfile, "\tmovq\t%%rax,%s\n", register_list[r1]);
    free_register(r2);
    return r1;
}

void cgprintint(int r) {
    fprintf(outfile, "\tmovq\t%s, %%rdi\n", register_list[r]);
    fprintf(outfile, "\tcall\tprintint\n");
    free_register(r);
}

void cgpreamble() {
  free_all_registers();
  /*
  fputs(
	"\t.text\n"
	".LC0:\n"
	"\t.string\t\"%d\\n\"\n"
	"printint:\n"
	"\tpushq\t%rbp\n"
	"\tmovq\t%rsp, %rbp\n"
	"\tsubq\t$16, %rsp\n"
	"\tmovl\t%edi, -4(%rbp)\n"
	"\tmovl\t-4(%rbp), %eax\n"
	"\tmovl\t%eax, %esi\n"
	"\tleaq	.LC0(%rip), %rdi\n"
	"\tmovl	$0, %eax\n"
	"\tcall	printf@PLT\n"
	"\tnop\n"
	"\tleave\n"
	"\tret\n"
	"\n",
  outfile);
  */
}

void cgpostamble() {
  fputs(
	"\tmovl	$0, %eax\n"
	"\tpopq	%rbp\n"
	"\tret\n",
  outfile);
}

void cgfunctionpreamble(int id) {
    char* name = global_symbols[id].name;
    fprintf(outfile,
        "\t.text\n"
        "\t.globl\t%s\n"
        "\t.type\t%s, @function\n"
        "%s:\n" "\tpushq\t%%rbp\n"
        "\tmovq\t%%rsp, %%rbp\n", name, name, name);
}

void cgfunctionpostamble(int id) {
    cglabel(global_symbols[id].endlabel);
    fputs("\tpopq %rbp\n" "\tret\n", outfile);
}

void generate_preamble() {cgpreamble();}
void generate_postamble() {cgpostamble();}
void generate_free_registers() {free_all_registers();}
void generate_printint(int reg) {cgprintint(reg);}

void cglabel(int l) {
    fprintf(outfile, "L%d:\n", l);
}

void cgjump(int l) {
    fprintf(outfile, "\tjmp\tL%d\n", l);
}

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

int cgaddress(int id) {
    int r = allocate_register();

    fprintf(outfile, "\tleaq\t%s(%%rip), %s\n", global_symbols[id].name, register_list[r]);
    return r;
}

int cgderef(int r, int type) {
    switch (type) {
        case P_CHARPTR:
            fprintf(outfile, "\tmovzbq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
        case P_INTPTR:
            fprintf(outfile, "\tmovq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
        case P_LONGPTR:
            fprintf(outfile, "\tmovq\t(%s), %s\n", register_list[r], register_list[r]);
            break;
    }
    return (r);
}

int cgshlconst(int r, int val) {
    fprintf(outfile, "\tsalq\t$%d, %s\n", val, register_list[r]);
    return r;
}