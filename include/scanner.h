#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "error.h"

#define TEXTLEN     512

enum {
    T_PLUS = 1,         // '+'
    T_MINUS,        // '-'
    T_STAR,         // '*'
    T_SLASH,        // '/'
    T_SEMICOLON,    // ';'
    T_ASSIGNMENT,   // '='
    T_COLON,        // ':'

    T_EQUALS,       // '=='
    T_NOT_EQUAL,    // '!='
    T_LESS_THAN,    // '<'
    T_LESS_EQUAL,   // '<='
    T_GREATER_THAN, // '>'
    T_GREATER_EQUAL,// '>='

    T_LEFT_BRACE,   // '{'
    T_RIGHT_BRACE,  // '}'
    T_LEFT_PAREN,   // '('
    T_RIGHT_PAREN,  // ')'

    T_AMPER,        // '&'

    T_COMMA,        // ','

    T_LEFT_BRACKET, // '['
    T_RIGHT_BRACKET,// ']'


    T_LOGIC_OR,     // '||'
    T_LOGIC_AND,    // '&&'
    T_OR,           // '|'
    T_XOR,          // '^'
    T_LSHIFT,       // '<<'
    T_RSHIFT,       // '>>'
    T_INCREMENT,    // '++'
    T_DECREMENT,    // '--'
    T_INVERT,       // '~'
    T_LOGIC_NOT,    // '!'

    T_DOT,          // '.'
    T_ARROW,        // '->'

    T_INTLIT,       // integer literal
    T_IDENTIFIER,   // representing an identifier
    T_STRINGLIT,    // representing a string literal

    T_STRUCT,       // 'struct' keyword
    T_UNION,        // 'union' keyword
    T_INT,          // 'int' keyword
    T_IF,           // 'if' keyword
    T_WHILE,        // 'while' keyword
    T_ELSE,         // 'else' keyword
    T_FOR,          // 'for' keyword
    T_VOID,         // 'void' keyword

    T_CHAR,         // 'char' keyword
    T_LONG,         // 'long' keyword
    T_RETURN,       // 'return' keyword
    T_TYPEDEF,      // 'typedef' keyword
    T_ENUM,         // 'enum' keyword

    T_AUTO,         // 'auto' keyword
    T_BREAK,        // 'break' keyword
    T_CASE,         // 'case' keyword
    T_CONST,        // 'const' keyword
    T_CONTINUE,     // 'continue' keyword
    T_DEFAULT,      // 'default' keyword
    T_DO,           // 'do' keyword
    T_FLOAT,        // 'float' keyword
    T_DOUBLE,       // 'double' keyword
    T_EXTERN,       // 'extern' keyword
    T_GOTO,         // 'goto' keyword
    T_REGISTER,     // 'register' keyword
    T_SIGNED,       // 'signed' keyword
    T_SIZEOF,       // 'sizeof' keyword
    T_STATIC,       // 'static' keyword
    T_SWITCH,       // 'switch' keyword
    T_UNSIGNED,     // 'unsigned' keyword
    T_VOLATILE,     // 'volatile' keyword

    T_EOF
};

typedef struct token {
    int token;
    int value;
} t_token;

extern FILE* infile;
extern int line;
extern int last_char;
extern char text[TEXTLEN + 1];

extern t_token token;
extern char* infile_name;

int scan(t_token* t);
void reject_token(t_token* t);

#endif