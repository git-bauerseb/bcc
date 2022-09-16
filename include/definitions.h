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
    A_XOR,

    A_BREAK,
    A_CONTINUE,

    A_SWITCH,
    A_DEFAULT,
    A_CASE
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

// Structural type of a symbol
enum {
    S_VARIABLE,
    S_FUNCTION,
    S_ARRAY
};

// Storage class for a symbol
enum {
    C_GLOBAL = 1,       // Globally visible
    C_LOCAL,            // Locally visible
    C_PARAMETER,        // Locally visible function parameter
    C_MEMBER,           // Member of a structure

    C_STRUCT,
    C_UNION,

    C_ENUM_VALUE,
    C_ENUM_TYPE,
    C_TYPEDEF,
    C_EXTERN            // Externally visible global variable
};

// Entry in the symbol table. Stores all relevant informations
// for function types/structs/unions/enums declared in the program.
typedef struct symbol_table {
    char* name;                     // Name of a symbol
    int type;                       // Primitive type for symbol
    // Not used if the symbol is of composite type

    struct symbol_table* ctype;     // Type for a symbol that refers to a composite type.
    int stype;                      // Structural type for symbol
    int class;                      // Storage class for the symbol

    union {
        int num_elements;           // For arrays, the number of elements in the array
        int endlabel;               // End label (the label to jump to) after function
    };

    union {
        int size;                   // Number of elements in the symbol
        int params;                 // Number of parameters for a function
        int offset;                 // Negative offset of base pointer for local variables
    };


    int* initializer_list;          // Scalars have one initial value
    // Arrays have several initial values

    struct symbol_table *next;      // Next symbol in list
    struct symbol_table *member;    // Parameter of function, struct, union, enum, ...

} t_symbol_entry;

typedef struct symbol_list {
    t_symbol_entry *head;
    t_symbol_entry *tail;
} t_symbol_list;

// Sructure used in the Abstract-Systax Tree (AST).
typedef struct astnode {
    int op;
    int type;
    int rvalue;
    struct astnode* left;
    struct astnode* middle;
    struct astnode* right;
    t_symbol_entry* symbol;
    union {
        int value;              // For A_INTLIT, the integer value
        int size;               // For A_SCALE, the size to scale by
    };
} t_astnode;

#endif