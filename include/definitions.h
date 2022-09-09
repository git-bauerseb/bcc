#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define NOREG (-1)
#define NOLABEL 0

// Enumeration of the different types of nodes stored in the AST.
enum {
    A_ADD = 0x100,
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
    A_DEREFERENCE,
    A_STRLIT,
    A_LSHIFT,
    A_RSHIFT,
    A_OR,                   // '|' operator
    A_AND,                  // '&' operator
    A_LOGIC_NOT,            // '!' operator

    A_POST_DECREMENT,       // <id>--
    A_POST_INCREMENT,       // <id>++
    A_NEGATE,               // Invert value
    A_PRE_INCREMENT,        // ++<id>
    A_PRE_DECREMENT,        // --<id>

    A_INVERT,
    A_XOR
};

// Enumeration of different types
// The primitive type is encoded in the lowest 4 bit
// 0: None
// 1: Void
// 2: Char
// 3: Short
// 4: Int
// 5: Long
// The kind of pointer is encoded in the next 4 bit
// 0001 0100    - Pointer to int (int*)
// 0010 0100    - Pointer to pointer to int (int**)
// 0100 0100    - Pointer to pointer to pointer to int (int***)
// ...
enum {
    TYPE_NONE = 0x0,
    TYPE_VOID = 0x1,
    TYPE_CHAR = 0x2,
    TYPE_SHORT = 0x3,
    TYPE_INT = 0x4,
    TYPE_LONG = 0x5,
    TYPE_STRUCT = 0x6,
    TYPE_UNION = 0x7
};

#endif