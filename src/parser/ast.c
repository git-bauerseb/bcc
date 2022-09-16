#include "../../include/ast.h"

// Helper Functions
static int isCompOperator(int tokenType);
static int type_compatible(int* left, int* right, int onlyright);

t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, t_symbol_entry* symbol, int value) {
    t_astnode *n;

    n = (t_astnode *)malloc(sizeof(t_astnode));

    if (n == NULL) {
        fprintf(stderr, "malloc() failed in make_astnode(...)\n");
        exit(1);
    }

    n->op = op;
    n->left = left;
    n->right = right;
    n->value = value;
    n->symbol = symbol;
    n->type = type;
    return n;
}

t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, t_symbol_entry* symbol, int value) {
    t_astnode *n;

    n = (t_astnode *)malloc(sizeof(t_astnode));

    if (n == NULL) {
        fprintf(stderr, "malloc() failed in make_astnode(...)\n");
        exit(1);
    }

    n->op = op;
    n->left = left;
    n->middle = middle;
    n->right = right;
    n->value = value;
    n->type = type;
    return n;
}

t_astnode* make_ast_leaf(int op, int type, t_symbol_entry* symbol, int value) {
    return make_astnode(op, type, NULL, NULL, symbol, value);
}

t_astnode* make_unary_ast_node(int op, int type, t_astnode *left, t_symbol_entry* symbol, int value) {
    return make_astnode(op, type, left, NULL, symbol, value);
}

int arithop(int tok) {
    switch (tok) {
        case T_PLUS:
            return A_ADD;
        case T_MINUS:
            return A_SUBTRACT;
        case T_STAR:
            return A_MULTIPLY;
        case T_SLASH:
            return A_DIVIDE;
        case T_EQUALS:
            return A_EQUALS;
        case T_NOT_EQUAL:
            return A_NOT_EQUAL;
        case T_GREATER_EQUAL:
            return A_GREATER_EQUAL;
        case T_GREATER_THAN:
            return A_GREATER_THAN;
        case T_ASSIGNMENT:
            return A_ASSIGN;
        case T_LESS_THAN:
            return A_LESS_THAN;
        case T_LESS_EQUAL:
            return A_LESS_EQUAL;

        // Left shift
        case T_LSHIFT:
            return A_LSHIFT;
        // Right shift
        case T_RSHIFT:
            return A_RSHIFT;
        case T_OR:
            return A_OR;
        // '&' operator
        case T_AMPER:
            return A_AND;
        case T_XOR:
            return A_XOR;
    default:
        fprintf(stderr, "Unknown token on line %d\n", line);
        exit(1);
    }
}

static int type_compatible(int* left, int* right, int onlyright) {
    // void values are never compatible
    if ((*left == TYPE_VOID) || (*right == TYPE_VOID)) {return 0;}

    // Same types are compatible
    if (*left == *right) {*left = *right = 0; return 1;}

    int leftsize, rightsize;
    leftsize = get_primitive_size(*left);
    rightsize = get_primitive_size(*right);

    // Types with size '0' are not compatible with any
    // other type
    if ((leftsize == 0) || (rightsize == 0)) {return 0;}

    if (leftsize < rightsize) {
        *left = A_WIDEN;
        *right = 0;
        return 1;
    }


    if (rightsize < leftsize) {
        if (onlyright) {
            return 0;
        }
        *right = A_WIDEN;
        *left = 0;
        return 1;
    }

    *left = *right = 0;
    return 1;
}

void match(int t, char* to_match) {
    if (token.token == t) {
        scan(&token);
    } else {
        fprintf(stderr, "%s expected on line %d\n", to_match, line);
        exit(1);
    }
}

