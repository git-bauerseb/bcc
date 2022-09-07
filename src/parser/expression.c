#include "../../include/ast.h"

// Expressions
// <prefix> ::= <primary>
//            | '*' <prefix>
//            | '&' <prefix>
//            | '-' <prefix>
//            | '++' <prefix>
//            | '--' <prefix>
static t_astnode* prefix(void);

// <postfix> ::= <primary>
//             | <postfix> '++'
//             | <postfix> '--'
static t_astnode* postfix(void);

// <expression_list> ::= <epsilon>
//                     | <expression>
//                     | <expression> ',' <expression_list>
static t_astnode* expression_list(void);

// <shift> ::= <comparison>
//                      | <comparison> ('<<' | '>>') <shift>
static t_astnode* shift_expression(void);

// <comparison> ::= <term>
//                | <term> ('<' | '<=' | '>' | '>=') <comparison>
static t_astnode* comparison_expression(void);

// <array_access> ::= '[' <expression> ']'
static t_astnode* array_access(void);

static t_astnode* assignment_expression(void);

// <equals> ::= <shift>
//            | <shift> ('==' | '!=') <equals>
static t_astnode* equals_expression(void);

// <term> ::= <factor>
//          | <factor> ('+' | '-') <term>
static t_astnode* term_expression(void);

// <factor> ::= <primary>
//            | <primary> ('*' | '/') <factor>
static t_astnode* factor_expression(void);

// <primary> ::= <number> | <string>
static t_astnode* primary(void);

// '|'
static t_astnode* or_expression(void);

// '&'
static t_astnode* and_expression(void);

// '^'
static t_astnode* xor_expression(void);

static void convert_types(t_astnode** left, t_astnode** right, int op);


t_astnode* binary_expression(void) {
    t_astnode* expr = assignment_expression();
    return expr;
}

static t_astnode* term_expression(void) {
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

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);

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

    left = comparison_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_LSHIFT || token.token == T_RSHIFT) {
        scan(&token);

        right = comparison_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

static t_astnode* factor_expression(void) {
    t_astnode* left, *right;
    int type;

    left = prefix();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while ((type == T_STAR) || (type == T_SLASH)) {
        scan(&token);
        right = prefix();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);

        type = token.token;

        if (type == T_EOF) {
            break;
        }
    }

    return left;
}

static t_astnode* postfix(void) {

    t_astnode* node;
    t_symbol_entry* variable;

    scan(&token);

    if (token.token == T_LEFT_PAREN) {
        return function_calls();
    }

    if (token.token == T_LEFT_BRACKET) {
        return array_access();
    }

    if ((variable = find_symbol(text)) == NULL || variable->stype != S_VARIABLE) {
        report_error("postfix(): Unknown variable %s\n", text);
    }
    
    switch (token.token) {
        case T_INCREMENT:
            scan(&token);
            node = make_ast_leaf(A_POST_INCREMENT, variable->type, variable, 0);
            break;
        case T_DECREMENT:
            scan(&token);
            node = make_ast_leaf(A_POST_DECREMENT, variable->type, variable, 0);
            break;
        default:
            node = make_ast_leaf(A_IDENTIFIER, variable->type, variable, 0);
            break;
    }

    return node;
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

        case T_DECREMENT:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER) {
                fprintf(stderr, "'++' stands before an identifier.\n");
                exit(1);
            }

            tree = make_ast_unary(A_PRE_DECREMENT, tree->type, tree, NULL, 0);
            break;

        case T_INCREMENT:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER) {
                fprintf(stderr, "'++' stands before an identifier.\n");
                exit(1);
            }

            tree = make_ast_unary(A_PRE_INCREMENT, tree->type, tree, NULL, 0);
            break;

        case T_INVERT:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = modify_type(tree, TYPE_INT, 0);
            tree = make_ast_unary(A_INVERT, tree->type, tree, NULL, 0);
            break;

        case T_LOGIC_NOT:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = make_ast_unary(A_LOGIC_NOT, tree->type, tree, NULL, 0);
            break;

        case T_MINUS:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = modify_type(tree, TYPE_INT, 0);
            tree = make_ast_unary(A_NEGATE, tree->type, tree, NULL, 0);
            break;

        case T_STAR:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER && tree->op != A_DEREFERENCE) {
                fprintf(stderr, "'*' must be followed by an identifier or '*'");
            }

            tree = make_ast_unary(A_DEREFERENCE, value_at(tree->type), tree, NULL, 0);
            break;

        default:
            tree = primary();
    }

    return tree;
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

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);

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

    left = term_expression();

    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token >= T_EQUALS && token.token <= T_GREATER_EQUAL) {
        scan(&token);

        right = term_expression();
        // Change this in every expression
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);
        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}


static t_astnode* xor_expression(void) {
    t_astnode* left, *right;
    int type;

    left = and_expression();
    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_XOR) {
        scan(&token);

        right = and_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);
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

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);
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

    left = xor_expression();
    type = token.token;

    if (type == T_SEMICOLON) {
        left->rvalue = 1;
        return left;
    }

    while (token.token == T_OR) {
        scan(&token);

        right = xor_expression();
        left->rvalue = right->rvalue = 1;
        convert_types(&left, &right, type);

        left = make_astnode(arithop(type), left->type, left, right, NULL, 0);
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

        // Pass arguments in different order to assure correct associativity
        left = make_astnode(arithop(type), left->type, right, left, NULL, 0);

        type = token.token;
        if (type == T_SEMICOLON) {
            return left;
        }
    }

    return left;
}

static t_astnode* primary(void) {
    t_astnode* n;
    int id;

    switch (token.token) {
        case T_STRINGLIT:
            id = generate_global_string(text);
            n = make_ast_leaf(A_STRLIT, pointer_to(TYPE_CHAR), NULL, id);
            break;
        case T_INTLIT:
            // Save as char if it fits in range
            if (token.value >= 0 && token.value < 256) {
                n = make_ast_leaf(A_INTLIT, TYPE_CHAR, NULL, token.value);
            } else {
                n = make_ast_leaf(A_INTLIT, TYPE_INT, NULL, token.value);
            }
            break;
        
        // Grouping '(' expression ')'
        case T_LEFT_PAREN:
            scan(&token);
            n = binary_expression();
            match(T_RIGHT_PAREN, "Expected ')'");
            return n;
        case T_IDENTIFIER:
            return postfix();
        default:
            fprintf(stderr, "Syntax error on line %d\n", line);
            exit(1);
    }

    scan(&token);
    return n;
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

static t_astnode* array_access(void) {
    t_astnode* left, *right;
    t_symbol_entry* symbol;

    if ((symbol = find_symbol(text)) == NULL || symbol->stype != S_ARRAY) {
        report_error("array_access(): Undeclared array %s.\n", text);
    }

    /*
        Array access is modeled as pointer addition:

        arr[5] --> *(arr + 5)
    */
    left = make_ast_leaf(A_ADDR, symbol->type, symbol, 0);

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

    left = make_astnode(A_ADD, symbol->type, left, right, NULL, 0);
    left = make_ast_unary(A_DEREFERENCE, value_at(left->type), left, NULL, 0);
    return left;
}

t_astnode* function_calls(void) {
    t_astnode* tree;
    t_symbol_entry* function_ptr;

    // Check if function name exists
    if ((function_ptr = find_symbol(text)) == NULL || function_ptr->type != S_FUNCTION) {
        report_error("function_calls(): Undeclared function %s.\n", text);
    }

    match(T_LEFT_PAREN, "(");

    tree = expression_list();
    tree = make_ast_unary(A_FUNCTION_CALL, function_ptr->type, tree, function_ptr, 0);
    match(T_RIGHT_PAREN, ")");

    return tree;
}

static t_astnode* expression_list(void) {
    t_astnode* tree, *child;
    tree = NULL;
    child = NULL;

    int expr_count = 0;

    while (token.token != T_RIGHT_PAREN) {
        child = binary_expression();
        expr_count++;

        tree = make_ternary_astnode(A_GLUE, TYPE_NONE, tree, NULL, child, NULL, expr_count);

        switch (token.token) {
            case T_COMMA:
                scan(&token);
                break;
            case T_RIGHT_PAREN:
                break;
            default:
                report_error("Unexpected token in expression list");
        }
    }

    return tree;
}