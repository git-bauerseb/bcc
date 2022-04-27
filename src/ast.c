#include "../include/ast.h"

/*
    Forward Declarations.
*/
static char* ast_names[] = {
    "A_ADD", "A_SUBTRACT", "A_MULTIPLY", "A_DIVIDE",
    "A_EQUALS", "A_NOT_EQUALS", "A_LESS_THAN", "A_GREATER_THAN",
    "A_LESS_EQUAL", "A_GREATER_EQUAL",
    "A_INTLIT", "A_IDENTIFIER", "A_LVIDENT",
    "A_ASSIGN", "A_PRINT", "A_GLUE", "A_IF", "A_WHILE",
    "A_FOR", "A_FUNCTION", "A_WIDEN", "A_SCALE", "A_FUNCTION_CALL",
    "A_RETURN", "A_ADDR", "A_DEREFERENCE"
};

// Parsing functions

// Functions
static t_astnode* function_calls(void);
static t_astnode* function_declaration(int type);


// Prefix operation ('*', '&')
static t_astnode* prefix(void);

// Statements
static t_astnode* print_statement(void);
static void var_declaration(int type);
static t_astnode* assignment_statement(void);
static t_astnode* while_statement(void);
static t_astnode* single_statement(void);
static t_astnode* compound_statement(void);
static t_astnode* return_statement(void);


// Expressions

/*
    <<, >> operators
*/
static t_astnode* shift_expression(void);

static t_astnode* array_access(void);
static t_astnode* assignment_expression(void);
static t_astnode* equals_expression(void);
static t_astnode* comparison_expression(void);
static t_astnode* term_expression(void);
static t_astnode* factor_expression(void);

static t_astnode* primary(void);

static t_astnode* or_expression(void);
static t_astnode* and_expression(void);

// Types
static int parse_type(int t);
static int get_primitive_size(int type);

// Helper Functions
static int isCompOperator(int tokenType);
static int type_compatible(int* left, int* right, int onlyright);
static void convert_types(t_astnode** left, t_astnode** right, int op);

static void print_ast(t_astnode* root, int depth);

static int inttype(int type);

static t_astnode* binary_expression(void) {
    t_astnode* expr = assignment_expression();

    return expr;
}

t_astnode* make_astnode(int op, int type, t_astnode* left, t_astnode* right, int value) {
    t_astnode *n;

    n = (t_astnode *)malloc(sizeof(t_astnode));

    if (n == NULL) {
        fprintf(stderr, "malloc() failed in make_astnode(...)\n");
        exit(1);
    }

    n->op = op;
    n->left = left;
    n->right = right;
    n->v.value = value;
    n->type = type;
    return n;
}

t_astnode* make_ternary_astnode(int op, int type, t_astnode* left, t_astnode* middle, t_astnode* right, int value) {
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
    n->v.value = value;
    n->type = type;
    return n;
}

t_astnode* make_ast_leaf(int op, int type, int value) {
    return make_astnode(op, type, NULL, NULL, value);
}

t_astnode* make_ast_unary(int op, int type, t_astnode *left, int value) {
    return make_astnode(op, type, left, NULL, value);
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
    default:
        fprintf(stderr, "Unknown token on line %d\n", line);
        exit(1);
    }
}

static int parse_type(int t) {
    int type;

    switch(token.token) {
        case T_VOID: type = P_VOID; break;
        case T_CHAR: type = P_CHAR; break;
        case T_INT: type = P_INT; break;
        case T_LONG: type = P_LONG; break;
        default:
            fprintf(stderr, "Illegal type, token %d\n", token.token);
    }

    while (1) {
        scan(&token);
        if (token.token != T_STAR) {break;}
        type = pointer_to(type);
    }

    return type;
}

static int get_primitive_size(int type) {
    return cgprimsize(type);
}


void global_declarations(void) {
    t_astnode* tree;
    int type;

    while (1) {
        type = parse_type(token.token);
        match(T_IDENTIFIER, "identifier");

        if (token.token == T_LEFT_PAREN) {
            tree = function_declaration(type);
            generate_ast(tree, NOREG, 0);
            // print_ast(tree, 0);
        } else {
            var_declaration(type);
        }

        if (token.token == T_EOF) {
            break;
        }
    }
}


static t_astnode* array_access(void) {
    t_astnode* left, *right;
    int id;

    if ((id = find_global(text)) == -1 || global_symbols[id].stype != S_ARRAY) {
        fprintf(stderr, "Undeclared array %s.\n", text);
        exit(1);
    }

    /*
        Array access is modeled as pointer addition:

        arr[5] --> *(arr + 5)
    */
    left = make_ast_leaf(A_ADDR, global_symbols[id].type, id);

    // '['
    match(T_LEFT_BRACKET, "Expected '[' for array access.");

    right = binary_expression();

    // ']'
    match(T_RIGHT_BRACKET, "Expected ']' after array access.");

    if (!inttype(right->type)) {
        fprintf(stderr, "Only integral value in array access allowed.");
        exit(1);
    }

    // Scale index by size of element type
    right = modify_type(right, left->type, A_ADD);

    left = make_astnode(A_ADD, global_symbols[id].type, left, right, 0);
    left = make_ast_unary(A_DEREFERENCE, value_at(left->type), left, 0);
    return left;
}

static t_astnode* primary(void) {
    t_astnode* n;
    int id;

    switch (token.token) {
        case T_STRINGLIT:
            id = generate_global_string(text);
            n = make_ast_leaf(A_STRLIT, P_CHARPTR, id);
            break;
        case T_INTLIT:
            // Save as char if it fits in range
            if (token.value >= 0 && token.value < 256) {
                n = make_ast_leaf(A_INTLIT, P_CHAR, token.value);
            } else {
                n = make_ast_leaf(A_INTLIT, P_INT, token.value);
            }
            break;
        
        // Grouping '(' expression ')'
        case T_LEFT_PAREN:
            scan(&token);
            n = binary_expression();
            match(T_RIGHT_PAREN, "Expected ')'");
            return n;
        case T_IDENTIFIER:

            // Distinguish between function call and variable
            // So look at next token
            scan(&token);

            if (token.token == T_LEFT_PAREN) {
                return function_calls();
            }

            if (token.token == T_LEFT_BRACKET) {
                return array_access();
            }

            reject_token(&token);

            id = find_global(text);

            if (id == -1) {
                fprintf(stderr, "Unknown variable %s.\n", text);
            }
            n = make_ast_leaf(A_IDENTIFIER, global_symbols[id].type, id);
            break;
        default:
            fprintf(stderr, "Syntax error on line %d\n", line);
            exit(1);
    }

    scan(&token);
    return n;
}


t_astnode* term_expression(void) {
    t_astnode* left, *right;
    int type;

    left = factor_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_PLUS || token.token == T_MINUS) {
        scan(&token);

        right = factor_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

static t_astnode* shift_expression(void) {
    t_astnode* left, *right;
    int type;

    left = term_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_LSHIFT || token.token == T_RSHIFT) {
        scan(&token);

        right = term_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

t_astnode* factor_expression(void) {
    t_astnode* left, *right;
    int type;

    left = prefix();

    type = token.token;

    if (type == T_EOF) {
        left->rvalue = 1;
        return left;
    }

    while ((type == T_STAR) || (type == T_SLASH)) {
        scan(&token);
        right = prefix();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);

        type = token.token;

        if (type == T_EOF) {
            break;
        }
    }

    return left;
}

static t_astnode* prefix(void) {
    t_astnode* tree;

    switch (token.token) {
        case T_AMPER:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER) {
                fprintf(stderr, "'&' must be followed by an identifier.\n");
            }

            tree->op = A_ADDR;
            tree->type = pointer_to(tree->type);
            break;
        case T_STAR:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER && tree->op != A_DEREFERENCE) {
                fprintf(stderr, "'*' must be followed by an identifier or '*'");
            }

            tree = make_ast_unary(A_DEREFERENCE, value_at(tree->type), tree, 0);
            break;
        default:
            tree = primary();
    }

    return tree;
}

static t_astnode* function_calls(void) {
    t_astnode* tree;
    int id;

    // Check if function name exists
    if ((id = find_global(text)) == -1) {
        fprintf(stderr, "Undeclared function %s.\n", text);
    }

    match(T_LEFT_PAREN, "(");

    tree = binary_expression();
    tree->rvalue = 1;
    tree = make_ast_unary(A_FUNCTION_CALL, global_symbols[id].type, tree, id);
    match(T_RIGHT_PAREN, ")");

    return tree;
}


static void convert_types(t_astnode** left, t_astnode** right, int op) {
    t_astnode* ltemp, *rtemp;

    ltemp = modify_type(*left, (*right)->type, op);
    rtemp = modify_type(*right, (*left)->type, op);

    if (ltemp == NULL && rtemp == NULL) {
        fprintf(stderr, "Incompatible types in binary expression.\n");
        exit(1);
    }

    if (ltemp != NULL) {*left = ltemp;}
    if (rtemp != NULL) {*right = rtemp;}
}


static int type_compatible(int* left, int* right, int onlyright) {
    // void values are never compatible
    if ((*left == P_VOID) || (*right == P_VOID)) {return 0;}

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


static t_astnode* equals_expression(void) {
    t_astnode* left, *right;
    int type;

    left = shift_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_EQUALS || token.token == T_NOT_EQUAL) {
        scan(&token);

        right = shift_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

static t_astnode* comparison_expression(void) {
    t_astnode* left, *right;
    int type;

    left = equals_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token >= T_EQUALS && token.token <= T_GREATER_EQUAL) {
        scan(&token);

        right = equals_expression();
        // Change this in every expression
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);
        left = make_astnode(arithop(type), left->type, left, right, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

static t_astnode* and_expression(void) {
    t_astnode* left, *right;
    int type;

    left = equals_expression();
    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_AMPER) {
        scan(&token);

        right = equals_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);
        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }

    }

    return left;
}


static t_astnode* or_expression(void) {
    t_astnode* left, *right;
    int type;

    left = and_expression();
    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_OR) {
        scan(&token);

        right = and_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, 0);
        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }

    }

    return left;
}


static t_astnode* assignment_expression(void) {
    t_astnode* left, *right;
    int type;

    left = or_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_ASSIGNMENT) {
        scan(&token);

        right = assignment_expression();
        // Here, only right (assignment target) is rvalue
        right->rvalue = 1;
        modify_type(right, left->type, 0);

        left = make_astnode(arithop(type), left->type, right, left, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}



void match(int t, char* to_match) {
    if (token.token == t) {
        scan(&token);
    } else {
        fprintf(stderr, "%s expected on line %d\n", to_match, line);
        exit(1);
    }
}



static void var_declaration(int type) {
    int id;

    // Text now has the identifier's name.
    // If the next token is a '['
    if (token.token == T_LEFT_BRACKET) {
    // Skip past the '['
    scan(&token);

    if (token.token == T_INTLIT) {
        id = add_global(text, pointer_to(type), S_ARRAY, 0, token.value);
        generate_global_symbol(id);
    }

    // Ensure we have a following ']'
    scan(&token);
    match(T_RIGHT_BRACKET, "]");
    } else {
    // Add this as a known scalar
    // and generate its space in assembly
    id = add_global(text, type, S_VARIABLE, 0, 1);
    generate_global_symbol(id);
    }

    // Get the trailing semicolon
    match(T_SEMICOLON, ";");
}

// ***********************************************************************
// STATEMENTS
// ***********************************************************************

// Print statement
static t_astnode* print_statement(void) {
    t_astnode *tree;
    int reg;
    int lefttype, righttype;
    
    match(T_PRINT, "print");

    tree = binary_expression();

    lefttype = P_INT;
    righttype = tree->type;

    if (!type_compatible(&lefttype, &righttype, 0)) {
        fprintf(stderr, "Incompatible types.\n");
    }

    if (righttype) {
        tree = make_ast_unary(righttype, P_INT, tree, 0);
    }

    tree = make_ast_unary(A_PRINT, P_NONE, tree, 0);
   
    return tree;
}

// Assignment
static t_astnode* assignment_statement(void) {
    t_astnode* left, *right, *tree;
    int id;

    match(T_IDENTIFIER, "identifier");

    if (token.token == T_LEFT_PAREN) {
        return function_calls();
    }

    if ((id = find_global(text)) == -1) {
        fprintf(stderr, "Undeclared variable %s\n", text);
        exit(1);
    }


   

    // Not reject token because either '(' or '=' are coming
    // after identifier

    right = make_ast_leaf(A_LVIDENT, global_symbols[id].type, id);
    match(T_ASSIGNMENT, "=");

    left = binary_expression();

    left = modify_type(left, right->type, 0);

    if (left == NULL) {
        fprintf(stderr, "Incompatible expression in assignment.\n");
        exit(1);
    }

    tree = make_astnode(A_ASSIGN, P_NONE, left, right, 0);

    return tree;
}

t_astnode* if_statement(void) {
    t_astnode* condAST, *trueAST, *falseAST = NULL;

    match(T_IF, "if");
    match(T_LEFT_PAREN, "(");

    condAST = binary_expression();

    if (condAST->op < A_EQUALS || condAST->op > A_GREATER_EQUAL) {
        fprintf(stderr, "Bad comparison operator.\n");
    }

    match(T_RIGHT_PAREN, ")");

    trueAST = compound_statement();

    // If there is an additional 'else' then skip this token
    // and parse conditional part
    if (token.token == T_ELSE) {
        scan(&token);
        falseAST = compound_statement();
    }

    return make_ternary_astnode(A_IF, P_NONE, condAST, trueAST, falseAST, 0);
}

static t_astnode* while_statement(void) {
    t_astnode* condAST, *bodyAST;

    match(T_WHILE, "while");
    match(T_LEFT_PAREN, "(");

    condAST = binary_expression();

    if (condAST->op < A_EQUALS || condAST->op > A_GREATER_EQUAL) {
        fprintf(stderr, "Bad comparison operator\n");
    }

    match(T_RIGHT_PAREN, ")");

    bodyAST = compound_statement();

    return make_astnode(A_WHILE, P_NONE, condAST, bodyAST, 0);
}

static t_astnode* for_statement() {
    t_astnode* init, *cond, *inc, *body, *tree;

    match(T_FOR, "for");
    match(T_LEFT_PAREN, "(");

    init = single_statement();
    match(T_SEMICOLON, ";");

    cond = binary_expression();
    match(T_SEMICOLON, ";");

    inc = single_statement();
    match(T_RIGHT_PAREN, ")");

    body = compound_statement();

    tree = make_astnode(A_GLUE, P_NONE, body, inc, 0);
    tree = make_astnode(A_WHILE, P_NONE, cond, tree, 0);
    tree = make_astnode(A_GLUE, P_NONE, init, tree, 0);

    return tree;
}

static t_astnode* single_statement() {
    int type;
    switch(token.token) {
        case T_PRINT:
            return print_statement(); 
        case T_INT:
        case T_CHAR:
            type = parse_type(token.token);
            match(T_IDENTIFIER, "identifier");
            var_declaration(type);
            return NULL;
        case T_IF:
            return if_statement();
        case T_WHILE:
            return while_statement();
        case T_FOR:
            return for_statement();
        case T_RETURN:
            return return_statement();
        default:
            return binary_expression();
            // fprintf(stderr, "Syntax error, token %d\n", token.token);
    }
}

static t_astnode* return_statement(void) {
    t_astnode* tree;
    int returntype, function_type;

    if (global_symbols[current_function_id].type == P_VOID) {
        fprintf(stderr, "Cannot return from a void function");
    }

    match(T_RETURN, "return");
    tree = binary_expression();
    tree = modify_type(tree, global_symbols[current_function_id].type, 0);

    if (tree == NULL) {
        fprintf(stderr, "Incompatible type to return.\n");
        exit(1);
    }

    tree = make_ast_unary(A_RETURN, P_NONE, tree, 0);

    return tree;
}

static t_astnode* function_declaration(int type) {
    t_astnode* tree, *finalstmt;
    int nameslot;

    int endlabel = label();

    nameslot = add_global(text, type, S_FUNCTION, endlabel, 0);
    current_function_id = nameslot;

    match(T_LEFT_PAREN, "(");
    match(T_RIGHT_PAREN, ")");

    tree = compound_statement();

    // If the type is not void then force a return statement
    if (type != P_VOID) {
        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;

        if (finalstmt == NULL || finalstmt->op != A_RETURN) {
            fprintf(stderr, "No return for function with non-void type.\n");
        }
    }

    return make_ast_unary(A_FUNCTION, P_NONE, tree, nameslot);
}

static t_astnode* compound_statement() {

    t_astnode* left = NULL;
    t_astnode* tree;

    match(T_LEFT_BRACE, "{");

    while (1) {
        
        tree = single_statement();

        if (tree != NULL 
            && (tree->op == A_ASSIGN
                || tree->op == A_FUNCTION_CALL || tree->op == A_RETURN)) {
            match(T_SEMICOLON, ";");
        }

        if (tree != NULL) {
            if (left == NULL) {
                left = tree;
            } else {
                left = make_astnode(A_GLUE, P_NONE, left, tree, 0);
            }
        }

        if (token.token == T_RIGHT_BRACE) {
            match(T_RIGHT_BRACE, "}");
            return left;
        }
    }
}

static int inttype(int type) {
    if (type == P_CHAR || type == P_INT || type == P_LONG) {
        return 1;
    }

    return 0;
}

static int pointer_type(int type) {
    if (type == P_VOIDPTR || type == P_CHARPTR || type == P_INTPTR || type == P_LONGPTR) {
        return 1;
    }

    return 0;
}


// ***********************************************************************
// HELPER FUNCTIONS
// ***********************************************************************
t_astnode* modify_type(t_astnode* tree, int rtype, int op) {
    int ltype;
    int lsize, rsize;

    ltype = tree->type;

    if (inttype(ltype) && inttype(rtype)) {
        // Same type; no change
        if (ltype == rtype) {return tree;}

        lsize = get_primitive_size(ltype);
        rsize = get_primitive_size(rtype);

        if (lsize > rsize) {return NULL;}

        // Widen to right type
        if (rsize > lsize) {return make_ast_unary(A_WIDEN, rtype, tree, 0);}
    }

    if (pointer_type(ltype)) {
        if (op == 0 && ltype == rtype) {return tree;}
    }

    // If left is int type, right is pointer type and size
    // of original type is > 1: scale the left.
    if (op == A_ADD || op == A_SUBTRACT) {
        if (inttype(ltype) && pointer_type(rtype)) {
            rsize = get_primitive_size(value_at(rtype));

            if (rsize > 1) {
                return make_ast_unary(A_SCALE, rtype, tree, rsize);
            } else {
                return tree;    // No need to scale
            }
        }
    }

    return NULL;
}

static void print_ast(t_astnode* root, int depth) {

    if (root == NULL) {return;}

    char prefBuff[depth+1];

    for (int i = 0; i < depth; i++) {prefBuff[i]=' ';}
    prefBuff[depth] = '\0';

    char* ast_name = ast_names[root->op-1];

    switch (root->op) {
        case A_ASSIGN: printf("%s%s\n", prefBuff, ast_name); break;
        case A_IDENTIFIER: printf("%s%s(%s) %s\n", prefBuff, ast_name, global_symbols[root->v.id].name, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_INTLIT: printf("%s%s(%d) %s\n", prefBuff, ast_name, root->v.value, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_FUNCTION: printf("%s%s\n", prefBuff, ast_name); break;
        case A_GLUE: printf("%s%s\n", prefBuff, ast_name); break;
        case A_RETURN: printf("%s%s\n", prefBuff, ast_name); break;
        case A_DEREFERENCE: printf("%s%s %s\n", prefBuff, ast_name, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_FUNCTION_CALL: printf("%s%s %s\n", prefBuff, ast_name, root->rvalue != 0 ? "rvalue" : ""); break;
    }

    if (root->left != NULL) {
        print_ast(root->left, root->op == A_WIDEN ? depth : depth+2);
    }

    if (root->right != NULL) {
        print_ast(root->right, root->op == A_WIDEN ? depth : depth+2);
    }
}