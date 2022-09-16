#include "../include/scanner.h"

/*
    Forward declarations
*/
static int next(void);
static int skip(void);
static void putback(int);
static int char_index(char*, int);
static int scan_int(int);

// Scan character
static int scanch(void);

// Scan string
static int scanstr(char* buff);

static int scan_identifier(int c, char* buff, int lim);

static t_token* rejected_token = NULL;

void reject_token(t_token* t) {
    if (rejected_token != NULL) {
        fprintf(stderr, "Only reject one token at a time.\n");
    }

    rejected_token = t;
}

static int skip(void) {
    int c;

    c = next();

    while (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
        c = next();
    }

    return c;
}

static int next(void) {
    int c;
    int l;

    if (last_char) {
        c = last_char;
        last_char = 0;
        return c;
    }

    c = fgetc(infile);

    while (c == '#') {
        scan(&token);

        if (token.token != T_INTLIT) {
            report_error("next(): Expected preprocessor line number.\n");
        }

        l = token.value;
        scan(&token);

        if (token.token != T_STRINGLIT) {
            report_error("next(): Expected file name.\n");
        }

        if (text[0] == '<') {
            if (strcmp(text, infile_name) != 0) {
                infile_name = strdup(text);
            }

            line = l;
        }

        while ((c = fgetc(infile)) != '\n') {c = fgetc(infile);}
    }

    if (c == '\n') {
        line++;
    }

    return c;
}

static void putback(int c) {
    last_char = c;
}

static int char_index(char *s, int c) {
    char* p;
    p = strchr(s, c);

    return (p ? p - s: -1);
}

static int scan_int(int c) {
    int pos, val = 0;

    while ((pos = char_index("0123456789", c)) >= 0) {
        val = val * 10 + pos;
        c = next();
    }

    putback(c);

    return val;
}

static int scan_identifier(int c, char* buff, int lim) {
    int i = 0;

    while (isalpha(c) || isdigit(c) || '_' == c) {
        if (i == (lim - 1)) {
            fprintf(stderr, "Identifier too long on line %d\n", line);
            exit(1);
        } else if (i < (lim - 1)) {
            buff[i++] = c;
        }

        c = next();
    }

    putback(c);
    buff[i] = '\0';
    return i;
}

static int keyword(char* s) {
    switch (*s) {
        case 'a':
            if (!strcmp(s, "auto")) { return T_AUTO; }
            break;
        case 'b':
            if (!strcmp(s, "break")) { return T_BREAK; }
            break;
        case 'c':
            if (!strcmp(s, "char")) {return T_CHAR;}
            else if (!strcmp(s, "continue")) { return T_CONTINUE; }
            else if (!strcmp(s, "const")) {return T_CONST;}
            else if (!strcmp(s, "case")) { return T_CASE; }
            break;
        case 'd':
            if (!strcmp(s, "default")) { return T_DEFAULT;}
            else if (!strcmp(s, "double")) { return T_DOUBLE;}
            else if (!strcmp(s, "do")) {return T_DO;}
            break;
        case 'e':
            if (!strcmp(s, "else")) {return T_ELSE;}
            else if (!strcmp(s, "enum")) {return T_ENUM;}
            else if (!strcmp(s, "extern")) {return T_EXTERN;}
            break;
        case 'f':
            if (!strcmp(s, "for")) {return T_FOR;}
            else if (!strcmp(s, "float")) {return T_FLOAT;}
            break;
        case 'g':
            if (!strcmp(s, "goto")) {return T_GOTO;}
            break;
        case 'i':
            if (!strcmp(s, "int")) {return T_INT;}
            else if (!strcmp(s, "if")) {return T_IF;}
            break;
        case 'l': {
            if (!strcmp(s, "long")) {return T_LONG;}
            break;
        }
        case 'r': {
            if (!strcmp(s, "return")) { return T_RETURN; }
            else if (!strcmp(s, "register")) {return T_REGISTER;}
            break;
        }
        case 's':
            if (!strcmp(s, "struct")) { return T_STRUCT; }
            else if (!strcmp(s, "sizeof")) {return T_SIZEOF;}
            else if (!strcmp(s, "static")) {return T_STATIC;}
            else if (!strcmp(s, "switch")) {return T_SWITCH;}
            else if (!strcmp(s, "signed")) {return T_SIGNED;}
            break;
        case 't':
            if (!strcmp(s, "typedef")) {return T_TYPEDEF;}
            break;
        case 'u':
            if (!strcmp(s, "union")) { return T_UNION;}
            else if (!strcmp(s, "unsigned")) {return T_UNSIGNED;}
            break;
        case 'v':
            if (!strcmp(s, "void")) { return T_VOID; }
            else if (!strcmp(s, "volatile")) { return T_VOLATILE;}
            break;
        case 'w':
            if (!strcmp(s, "while")) { return T_WHILE; }
            break;
    }

    return 0;
}

int scan(t_token* t) {
    int c;

    if (rejected_token != NULL) {
        t = rejected_token;
        rejected_token = NULL;
        return 1;
    }

    c = skip();

    switch (c) {
        case EOF:
            t->token = T_EOF;
            return 0;
        case '^':
            t->token = T_XOR;
            break;
        case ':':
            t->token = T_COLON;
            break;
        case '+':
            if ((c = next()) == '+') {
                t->token = T_INCREMENT;
            } else {
                putback(c);
                t->token = T_PLUS;
            }
            break;
        case '-':
            if ((c = next()) == '-') {
                t->token = T_DECREMENT;
            } else if (c == '>') {
                t->token = T_ARROW;
            } else {
                putback(c);
                t->token = T_MINUS;
            }
            break;
        case '.':
            t->token = T_DOT;
            break;
        case '|':
            if ((c = next()) == '|') {
                t->token = T_LOGIC_OR;
            } else {
                putback(c);
                t->token = T_OR;
            }
            break;
        case '~':
            t->token = T_INVERT;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        case ';':
            t->token = T_SEMICOLON;
            break;
        case ',':
            t->token = T_COMMA;
            break;
        case '=':
            if ((c = next()) == '=') {
                t->token = T_EQUALS;
            } else {
                putback(c);
                t->token = T_ASSIGNMENT;
            }
            break;
        case '&':
            if ((c = next()) == '&') {
                t->token = T_LOGIC_AND;
            } else {
                putback(c);
                t->token = T_AMPER;
            }
            break;
        case '!':
            if ((c = next()) == '=') {
                t->token = T_NOT_EQUAL;
            } else {
                putback(c);
                t->token = T_LOGIC_NOT;
            }
            break;
        case '<':
            if ((c = next()) == '=') {
                t->token = T_LESS_EQUAL;
            } else if (c == '<'){
                t->token = T_LSHIFT;
            } else {
                putback(c);
                t->token = T_LESS_THAN;  
            }
            break;
        case '>':
            if ((c = next()) == '=') {
                t->token = T_GREATER_EQUAL;
            } else if (c == '>'){
                t->token = T_RSHIFT;
            } else {
                putback(c);
                t->token = T_GREATER_THAN;  
            }
            break;
        case '(': 
            t->token = T_LEFT_PAREN;
            break;
        case ')':
            t->token = T_RIGHT_PAREN;
            break;
        case '{':
            t->token = T_LEFT_BRACE;
            break;
        case '}':
            t->token = T_RIGHT_BRACE;
            break;
        case '[':
            t->token = T_LEFT_BRACKET;
            break;
        case ']':
            t->token = T_RIGHT_BRACKET;
            break;
        case '\'':
            t->value = scanch();
            t->token = T_INTLIT;
            if (next() != '\'') {
                fprintf(stderr, "Expecte '\'' at end of character.\n");
                exit(1);
            }
            break;
        case '"':
            scanstr(text);
            t->token = T_STRINGLIT;
            break;
        default:
            if (isdigit(c)) {
                t->value = scan_int(c);
                t->token = T_INTLIT;
                break;
            } else if (isalpha(c) || c == '_') {
                scan_identifier(c, text, TEXTLEN);
                int type;

                if ((type = keyword(text))) {
                    t->token = type;
                    break;
                }

                t->token = T_IDENTIFIER;
                break;
            }

            fprintf(stderr, "Unrecognized character %c on line %d\n", c, line);
            exit(1);
    }

    return 1;
}


static int scanch(void) {
    int c;
    c = next();

    if (c == '\\') {
        switch (c = next()) {
            case 'n': return '\n';
            case '\\': return '\\';
            case '\'': return '\'';
            case '0' : return '\0';
            default:
                fprintf(stderr, "Unknown escape sequence for %c.\n", c);
                exit(1);
        }
    }

    return c;
}

static int scanstr(char* buff) {
    int i,c;

    for (i = 0; i < TEXTLEN - 1; i++) {
        // End string here
        if ((c = scanch()) == '"') {
            buff[i] = 0;
            return i;
        }

        buff[i] = c;
    }

    fprintf(stderr, "String literal too long.\n");
    exit(1);
    return 0;
}