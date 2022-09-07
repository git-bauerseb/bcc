#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "../include/scan.h"
#include "../include/ast.h"

#define MAX_OBJECTS 100


#ifdef DEBUG
    #include "../include/test.h"
#endif

char* object_files[MAX_OBJECTS];
int object_count = 0;

static char* output_name;

// Different options that can be given to the compiler
// F_COMPILE: Convert source to assembly code
// F_ASSEMBLE: Convert assembly to object files
// F_LINK: Link object files to an executable
enum {
    F_COMPILE = 0x1,
    F_ASSEMBLE = 0x2,
    F_LINK = 0x8,
    F_HELP = 0x400,
    F_VERBOSE = 0x800,
    F_AST_PRINT = 0x1600
};

const char* usage_string =
"Usage: ./bcc [-vchST] [-o output_name] file [file ...]\n"
"       -c generate object files but don't link\n"
"       -S compile but neither assemble nor link\n"
"       -T print syntax tree to stdout\n"
"       -h print this message to stdout\n"
"       -v print verbose output of all stages\n";

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

t_symbol_entry* function_id;

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

static int process_args(int argc, char** argv, int* last_idx) {

    int flags = F_LINK | F_COMPILE | F_ASSEMBLE;

    for (int i = 1; i < argc; i++) {


        if (argv[i][0] != '-') {
            *last_idx = i;
            break;
        }

        int arg_length = strlen(argv[i]);

        for (int j = 1; j < arg_length; j++) {
            switch (argv[i][j]) {
                case 'o':
                    output_name = argv[++i];
                    break;
                case 'c':
                    // Do everything except link
                    flags |= F_ASSEMBLE;
                    flags |= F_COMPILE;
                    flags ^= F_LINK;
                    break;
                case 'S':
                    // Do everything except object files and link
                    flags |= F_COMPILE;
                    flags ^= F_LINK;
                    flags ^= F_ASSEMBLE;
                    break;
                case 'h':
                    flags |= F_HELP;
                    break;
                case 'v':
                    flags |= F_VERBOSE;
                    break;
                case 'T':
                    flags |= F_AST_PRINT;
                    break;
            }
        }
    }

    return flags;
}

char* alter_suffix(const char* str, char n_suffix) {
    char* n_str;

    if (str == NULL) {return NULL;}

    int idx = strlen(str);
    while (idx >= 0 && str[idx] != '.') {idx--;}

    if (idx < 0 || str[idx] != '.') {
        return NULL;
    }

    n_str = malloc(sizeof(char)*idx+2);
    n_str[idx+2] = '\0';
    n_str[idx+1] = n_suffix;
    n_str[idx] = '.';

    for (int i = 0; i < idx; i++) {n_str[i] = str[i];}
    return n_str;
}


// Convert C source code to assembly
// Given an input filename, compile the file to assembly code
// and return name of file that contains assembly code.
// The new name of the assembled file is the old filename with
// the suffix replaced by '.s'
static char* do_compile(char* filename) {
    char* outfile_name = alter_suffix(filename, 's');

    if (outfile_name == NULL) {
        report_error("Error: Provided file %s has no suffix, use .c\n", filename);
    }

    if ((infile = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", filename, strerror(errno));
        exit(1);
    }

    if ((outfile = fopen(outfile_name, "w")) == NULL) {
        fprintf(stderr, "Unable to open %s: %s\n", outfile_name, strerror(errno));
        exit(1);
    }

    line = 1;
    clear_symbol_table();
    scan(&token);
    generate_preamble();
    global_declarations();
    fclose(outfile);

    return outfile_name;
}

// Convert assembly to object file
char* do_assemble(const char* filename) {
    char cmd[TEXTLEN];
    int err;

    char *outfilename = alter_suffix(filename, 'o');

    if (outfilename == NULL) {
        report_error("Error: %s has no suffix, use .s\n", filename);
    }

    snprintf(cmd, TEXTLEN, "%s %s %s", "as -o", outfilename, filename);
    err = system(cmd);

    if (err != 0) {
        report_error("Assembly of %s failed\n", filename);
    }

    return outfilename;
}

void do_link() {
    int cnt;
    int size = TEXTLEN;

    char cmd[TEXTLEN];
    char* cptr;
    int err;

    cptr = cmd;
    cnt = snprintf(cptr, size, "%s %s ", "cc -o", output_name);
    cptr += cnt;
    size -= cnt;

    char** objs = object_files;

    while (*objs != NULL) {
        cnt = snprintf(cptr, size, "%s ", *objs);
        cptr += cnt;
        size -= cnt;
        objs++;
    }

    err = system(cmd);

    if (err != 0) {
        report_error("Linking failed\n");
    }
}

int main(int argc, char** argv) {

#ifdef DEBUG
    test_types();
#endif

#ifndef DEBUG
    int l_idx;
    int flags = process_args(argc, argv, &l_idx);

    
    if (flags & F_HELP) {
        printf("%s", usage_string);
        exit(0);
    }

    setup_symbol_table();

    while (l_idx < argc) {
        char* asm_file = do_compile(argv[l_idx]);

        if ((flags & F_LINK) || (flags & F_ASSEMBLE)) {
            char* obj_file = do_assemble(asm_file);

            object_files[object_count] = obj_file;
            object_count++;
            object_files[object_count] = NULL;
        }

        if ((flags & F_LINK)) {
            do_link();
        }

        // If we link or assemble file we don't want assembly output
        if ((flags & F_LINK) || (flags & F_ASSEMBLE)) {
            unlink(asm_file);
        }

        // If we link output, we don't want single object files
        if (flags & F_LINK) {
            for (int i = 0; object_files[i] != NULL; i++) {
                unlink(object_files[i]);
            }
        }

        l_idx++;
    }
#endif
    exit(0);
    return 0;
}