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
//             | <postfix> '.'
//             | <postfix> '->'
static t_astnode* postfix(void);


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

static t_astnode* member_access(int indirect) {

    t_astnode* left, *right;
    t_symbol_entry* composit;
    t_symbol_entry* member;

    if ((composit = find_symbol(text)) == NULL) {
        report_error("member_access(): Error, accessing member of non-existing variable %s.\n", text);
    }

    // If we try to access a member of a direct struct with '->' we report an error
    if (indirect && composit->type != pointer_to(TYPE_STRUCT)
        && composit->type != pointer_to(TYPE_UNION)) {
        report_error("member_access(): Expected pointer to struct, got struct.\n");
    }

    if (!indirect && composit->type != TYPE_STRUCT && composit->type != TYPE_UNION) {
        report_error("member_access(): Expected struct, got something else.\n");
    }

    if (indirect) {
        if (composit->type == TYPE_STRUCT) {
            left = make_ast_leaf(A_IDENTIFIER, pointer_to(TYPE_STRUCT), composit, 0);
        } else {
            left = make_ast_leaf(A_IDENTIFIER, pointer_to(TYPE_UNION), composit, 0);
        }
    } else {
        left = make_ast_leaf(A_ADDR, composit->type, composit, 0);
    }

    t_symbol_entry* ctype = composit->ctype;
    left->rvalue = 1;
    scan(&token);
    if (indirect) {
        match(T_IDENTIFIER, "Expected identifier after '->'.\n");
    } else {
        match(T_IDENTIFIER, "Expected identifier after '.'.\n");
    }

    t_symbol_entry* m;

    for (m = ctype->member; m != NULL; m = m->next) {
        if (!strcmp(m->name, text)) {
            break;
        }
    }

    if (m == NULL) {
        report_error("member_access(): No member %s in struct %s\n", text, composit->name);
    }

    // Get offset of struct member
    right = make_ast_leaf(A_INTLIT, TYPE_INT, NULL, m->offset);

    left = make_astnode(A_ADD, pointer_to(m->type), left, right, NULL, 0);
    left = make_unary_ast_node(A_DEREFERENCE, m->type, left, NULL, 0);

    return left;
}


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
    t_symbol_entry* e_entry;

    if ((e_entry = find_enum_value(text)) != NULL) {
        scan(&token);
        return make_ast_leaf(A_INTLIT, TYPE_INT, NULL, e_entry->size);
    }

    scan(&token);

    if (token.token == T_LEFT_PAREN) {
        return function_calls();
    }

    if (token.token == T_LEFT_BRACKET) {
        return array_access();
    }

    if (token.token == T_DOT) {
        return (member_access(0));
    }

    if (token.token == T_ARROW) {
        return (member_access(1));
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

            tree = make_unary_ast_node(A_PRE_DECREMENT, tree->type, tree, NULL, 0);
            break;

        case T_INCREMENT:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER) {
                fprintf(stderr, "'++' stands before an identifier.\n");
                exit(1);
            }

            tree = make_unary_ast_node(A_PRE_INCREMENT, tree->type, tree, NULL, 0);
            break;

        case T_INVERT:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = modify_types(tree, TYPE_INT, 0);
            tree = make_unary_ast_node(A_INVERT, tree->type, tree, NULL, 0);
            break;

        case T_LOGIC_NOT:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = make_unary_ast_node(A_LOGIC_NOT, tree->type, tree, NULL, 0);
            break;

        case T_MINUS:
            scan(&token);
            tree = prefix();
            tree->rvalue = 1;
            tree = modify_types(tree, TYPE_INT, 0);
            tree = make_unary_ast_node(A_NEGATE, tree->type, tree, NULL, 0);
            break;

        case T_STAR:
            scan(&token);
            tree = prefix();

            if (tree->op != A_IDENTIFIER && tree->op != A_DEREFERENCE) {
                fprintf(stderr, "'*' must be followed by an identifier or '*'");
            }

            tree = make_unary_ast_node(A_DEREFERENCE, value_at(tree->type), tree, NULL, 0);
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
        modify_types(right, left->type, 0);

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

    ltemp = modify_types(*left, (*right)->type, op);
    rtemp = modify_types(*right, (*left)->type, op);

    if (ltemp == NULL && rtemp == NULL) {
        fprintf(stderr, "Incompatible types in binary expression.\n");
        exit(1);
    }

    if (ltemp != NULL) {*left = ltemp;}
    if (rtemp != NULL) {*right = rtemp;}
}

static t_astnode* array_access(void) {
    t_astnode* left, *right;
    t_symbol_entry* array;

    if ((array = find_symbol(text)) == NULL || array->stype != S_ARRAY) {
        report_error("array_access(): Undeclared array %s.\n", text);
    }

    left = make_ast_leaf(A_ADDR, array->type, array, 0);

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
    right = modify_types(right, left->type, A_ADD);

    left = make_astnode(A_ADD, array->type, left, right, NULL, 0);
    left = make_unary_ast_node(A_DEREFERENCE, value_at(left->type), left, NULL, 0);
    return left;
}

t_astnode* function_calls(void) {
    t_astnode* tree;
    t_symbol_entry* function_ptr;

    // Check if function name exists
    if ((function_ptr = find_symbol(text)) == NULL || function_ptr->stype != S_FUNCTION) {
        report_error("function_calls(): Undeclared function %s.\n", text);
    }

    match(T_LEFT_PAREN, "(");

    tree = expression_list(T_RIGHT_PAREN);
    tree = make_unary_ast_node(A_FUNCTION_CALL, function_ptr->type, tree, function_ptr, 0);
    match(T_RIGHT_PAREN, ")");

    return tree;
}

t_astnode* expression_list(int end_token) {
    t_astnode* tree, *child;
    tree = NULL;
    child = NULL;

    int expr_count = 0;

    while (token.token != end_token) {
        child = binary_expression();
        child->rvalue = 1;
        expr_count++;

        tree = make_ternary_astnode(A_GLUE, TYPE_NONE, tree, NULL, child, NULL, expr_count);

        if (token.token == end_token) {
            break;
        }

        match(T_COMMA, "expression_list(): ',' expected");
    }

    return tree;
}