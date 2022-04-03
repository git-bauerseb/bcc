#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define NOREG (-1)

enum {
    A_ADD = 1,
    A_SUBTRACT,
    A_MULTIPLY,
    A_DIVIDE,
    A_EQUALS,
    A_NOT_EQUAL,
    A_LESS_THAN,
    A_GREATER_THAN,
    A_LESS_EQUAL,
    A_GREATER_EQUAL,
    A_INTLIT,
    A_IDENTIFIER,
    A_LVIDENT,
    A_ASSIGN,
    A_PRINT,
    A_GLUE,                 //          Value: 16
    A_IF,
    A_WHILE,
    A_FOR,
    A_FUNCTION,
    A_WIDEN,

    A_SCALE,

    A_FUNCTION_CALL,
    A_RETURN,
    A_ADDR,
    A_DEREFERENCE
};

/*
    Type enum
*/
enum {
    P_NONE,
    P_VOID,
    P_CHAR,
    P_INT,
    P_LONG,

    P_VOIDPTR,          // void pointer
    P_CHARPTR,          // char pointer
    P_INTPTR,           // int pointer
    P_LONGPTR           // long pointer
};

#endif