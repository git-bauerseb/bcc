#include <stdlib.h>

#include "../include/scan.h"
#include "../include/ast.h"
#include "../include/interpret.h"
#include "../include/generate.h"
#include "../include/cg.h"
#include "../include/symbol.h"

char* token_names[] = {
    "T_PLUS",
    "T_MINUS",
    "T_STAR",
    "T_SLASH",
    "T_INTLIT",
    "T_SEMICOLON",
    "T_PRINT",
    "T_INT",
    "T_ASSIGNMENT",
    "T_IDENTIFIER",

    "T_EQUALS",
    "T_NOT_EQUALS",
    "T_LESS_THAN",
    "T_LESS_EQUAL",
    "T_GREATER_THAN",
    "T_GREATER_EQUAL",

    "T_LEFT_BRACE",
    "T_RIGHT_BRACE",
    "T_LEFT_PAREN",
    "T_RIGHT_PAREN",


    "T_IF",
    "T_WHILE",
    "T_ELSE",
    "T_FOR",
    "T_VOID",

    "T_CHAR",
    "T_LONG",
    "T_RETURN",
    "T_AMPER",
    "T_COMMA",

    "T_LEFT_BRACKET",
    "T_RIGHT_BRACKET",
    "T_STRINGLIT",
    "T_LOGIC_OR",
    "T_LOGIC_AND",
    "T_OR",
    "T_XOR",
    "T_LSHIFT",
    "T_RSHIFT",
    "T_INCREMENT",
    "T_DECREMENT",
    "T_INVERT",
    "T_LOGIC_NOT",

    "T_EOF"
};

t_symbol_entry* sym_table;

FILE* infile;

// File where assembly instructions are printed to
FILE* outfile;

// Buffer for holding identifiers during scaning
char text[TEXTLEN+1];

int last_char;
int line;

int current_function_id;

int global_next_pos = 0;
int local_next_pos = NUM_SYMBOLS - 1;

t_token token;

static void init() {
    last_char = 0;
    line = 0;
    free_all_registers();

    sym_table = (t_symbol_entry*)calloc(sizeof(t_symbol_entry), NUM_SYMBOLS);
}

static void scanner_test() {
    // Assumed: infile = fopen(argv[1], "r");
    scan(&token);

    while (token.token != T_EOF) {
        printf("TOKEN: %s\n", token_names[token.token-1]);
        scan(&token);
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