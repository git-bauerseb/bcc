#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TEXTLEN     512

enum {
    T_PLUS = 1,         // '+'
    T_MINUS,        // '-'
    T_STAR,         // '*'
    T_SLASH,        // '/'
    T_INTLIT,       // Integer literal
    T_SEMICOLON,    // ';'
    T_PRINT,        // 'print' keyword
    T_INT,          // 'int' keyword
    T_ASSIGNMENT,   // '='
    T_IDENTIFIER,   // representing an identifier           Value: 10

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

    T_IF,           // 'if' keyword                      Value: 20
    T_WHILE,        // 'while' keyword
    T_ELSE,         // 'else' keyword
    T_FOR,          // 'for' keyword
    T_VOID,         // 'void' keyword

    T_CHAR,         // 'char' keyword
    T_LONG,         // 'long' keyword
    T_RETURN,       // 'return' keyword

    T_AMPER,        // '&'

    T_COMMA,        // ','

    T_LEFT_BRACKET, // '['
    T_RIGHT_BRACKET,// ']'

    T_STRINGLIT,    // Representing a string literal

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

    T_STRUCT,       // 'struct'

    T_DOT,          // '.'
    T_ARROW,        // '->'

    T_UNION,

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

int scan(t_token* t);
void reject_token(t_token* t);

#endif