#include <stdlib.h>

#include "../include/scan.h"
#include "../include/ast.h"
#include "../include/interpret.h"
#include "../include/generate.h"
#include "../include/cg.h"
#include "../include/symbol.h"

t_symbol_entry* global_symbols;

char* tokstr[] = {"+", "-", "*", "/", "intlit"};

FILE* infile;

// File where assembly instructions are printed to
FILE* outfile;

// Buffer for holding identifiers during scaning
char text[TEXTLEN+1];

int last_char;
int line;

int current_function_id;

t_token token;

static void init() {
    last_char = 0;
    line = 0;
    free_all_registers();

    global_symbols = (t_symbol_entry*)calloc(sizeof(t_symbol_entry), NUM_SYMBOLS);
}

static void scanfile() {
    t_token t;

    while (scan(&t)) {
        printf("Token %s", tokstr[t.token]);
        if (t.token == T_INTLIT) {
            printf(", value %d", t.value);
        }

        if (t.token == T_PRINT) {
            printf(", print");
        }

        if (t.token == T_SEMICOLON) {
            printf(", ;");
        }

        putchar('\n');
    }
}

int main(int argc, char** argv) {

    t_astnode* tree;

    init();
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");

    add_global("printint", P_CHAR, S_FUNCTION, 0, 0);
    add_global("printchar", P_VOID, S_FUNCTION, 0, 0);

    scan(&token);
    generate_preamble();

    global_declarations();

    fclose(outfile);
    exit(0);

    return 0;
}