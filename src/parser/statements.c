#include "../../include/ast.h"

static t_astnode* switch_statement() {

    t_astnode* expr = NULL;
    t_astnode* node = NULL;

    t_astnode* case_tree = NULL;
    t_astnode* case_tail = NULL;

    t_astnode* left = NULL;

    int case_count = 0;
    int loop_cond = 1;

    int ast_type = 0;

    int default_declared = 0;
    int case_value = 0;

    // Scan 'switch'
    scan(&token);
    match(T_LEFT_PAREN, "switch_statement(): Expect '(' after 'switch'.\n");

    expr = binary_expression();

    match(T_RIGHT_PAREN, "switch_statement(): Expect ')' after switch expression.\n");
    match(T_LEFT_BRACE, "switch_statement(): Expect '{'.\n");

    if (!inttype(expr->type)) {
        report_error("switch_statement(): Expression in switch must be an integral type.\n");
    }

    node = make_unary_ast_node(A_SWITCH, 0, expr, NULL, 0);

    switch_level++;

    while (loop_cond) {
        switch (token.token) {
            case T_RIGHT_BRACE:
                if (case_count == 0) {
                    report_error("switch_statement(): No case in switch statement.\n");
                }
                loop_cond = 0;
                break;

            case T_CASE:
            case T_DEFAULT:
                if (default_declared) {
                    report_error("switch_statement(): Multiple default cases specified.\n");
                }

                if (token.token == T_DEFAULT) {
                    ast_type = A_DEFAULT;
                    default_declared = 1;
                    scan(&token);
                } else {
                    ast_type = A_CASE;
                    // Scan 'case'
                    scan(&token);

                    left = binary_expression();

                    if (left->op != A_INTLIT) {
                        report_error("switch_statement(): Only integer type in case.\n");
                    }

                    case_value = left->value;

                    t_astnode * c;

                    // Check for existing case values
                    for (c = case_tree; c != NULL; c = c->right) {
                        if (c->value == case_value) {
                            report_error("switch_statement(): Duplicate case in same switch statement.\n");
                        }
                    }
                }


                match(T_COLON, "Expect ':' after case declaration.\n");
                left = compound_statement(1);
                case_count++;

                if (case_tree == NULL) {
                    case_tree = case_tail = make_astnode(ast_type, 0, left, NULL, NULL, case_value);
                } else {
                    case_tail->right = make_astnode(ast_type, 0, left, NULL, NULL, case_value);
                    case_tail = case_tail->right;
                }

                break;

            default:
                report_error("switch_statement(): Unexpected token.\n");
        }
    }

    switch_level--;
    node->value = case_count;
    node->right = case_tree;

    match(T_RIGHT_BRACE, "Expect '}' after switch statement.\n");
    return node;
}

// <break> ::= 'break' ';'
static t_astnode* break_statement() {
    if (loop_level == 0 && switch_level == 0) {
        report_error("break_statement(): Cannot use 'break' outside loop context.\n");
    }

    scan(&token);
    return make_ast_leaf(A_BREAK, 0, NULL, 0);
}


// <continue> ::= 'continue' ';'
static t_astnode* continue_statement() {
    if (loop_level == 0) {
        report_error("continue_statement(): Cannot use 'continue' outside loop context.\n");
    }

    scan(&token);
    return make_ast_leaf(A_CONTINUE, 0, NULL, 0);
}

t_astnode* compound_statement(int is_switch_stmt) {

    t_astnode* left = NULL;
    t_astnode* tree;


    while (1) {
        tree = single_statement();

        if (tree != NULL) {
            if (left == NULL) {
                left = tree;
            } else {
                left = make_astnode(A_GLUE, 0, left, tree, NULL, 0);
            }
        }

        if (token.token == T_RIGHT_BRACE) { return left; }

        if (is_switch_stmt && (token.token == T_CASE || token.token == T_DEFAULT)) {return left;}
    }
}

// <for-statement> ::= 'for' '(' <expression-list> ';'
//                               <expression-list> ';'
//                               <expression-list> ')
//                      <compound-statement>
static t_astnode* for_statement() {
    t_astnode* init, *cond, *inc, *body, *tree;

    match(T_FOR, "for");
    scan(&token);


    init = expression_list(T_SEMICOLON);
    match(T_SEMICOLON, "Expect ';' after for loop initialization");

    cond = binary_expression();
    match(T_SEMICOLON, "Expect ';' after for loop condition");

    inc = expression_list(T_RIGHT_PAREN);
    match(T_RIGHT_PAREN, "Expect ')' after for loop header");

    loop_level++;
    body = single_statement();
    loop_level--;

    tree = make_astnode(A_GLUE, TYPE_NONE, body, inc, NULL, 0);
    tree = make_astnode(A_WHILE, TYPE_NONE, cond, tree, NULL, 0);
    tree = make_astnode(A_GLUE, TYPE_NONE, init, tree, NULL, 0);

    return tree;
}

t_astnode* single_statement() {
    int type;
    int class = C_LOCAL;
    t_symbol_entry* ctype;
    t_astnode* stmt;

    switch(token.token) {
        case T_LEFT_BRACE:
            match(T_LEFT_BRACE, "single_statement()");
            t_astnode* stmt = compound_statement(0);
            match(T_RIGHT_BRACE, "single_statement(): Expected '}'");
            return stmt;
        case T_IDENTIFIER:
            if (find_typedef_symbol(text) == NULL) {
               stmt = binary_expression();
               match(T_SEMICOLON, "Expect semicolon");
               return stmt;
            }
        case T_INT:
        case T_CHAR:
        case T_LONG:
        case T_STRUCT:
        case T_ENUM:
        case T_UNION:
        case T_EXTERN:
        case T_TYPEDEF:
            declaration_list(&ctype, C_LOCAL, T_SEMICOLON, T_EOF);           // Parse local variables
            match(T_SEMICOLON, ";");
            return NULL;
        case T_IF:
            return if_statement();
        case T_WHILE:
            return while_statement();
        case T_FOR:
            return for_statement();
        case T_RETURN:
            return return_statement();
        case T_BREAK:
            return break_statement();
        case T_CONTINUE:
            return continue_statement();

        case T_SWITCH:
            return switch_statement();
        default:
            stmt = binary_expression();
            match(T_SEMICOLON, "Expect ';'");
            return stmt;
    }
}

t_astnode* return_statement(void) {
    t_astnode* tree;
    int returntype, function_type;

    if (function_id->type == TYPE_VOID) {
        fprintf(stderr, "Cannot return from a void function");
    }

    match(T_RETURN, "return");
    tree = binary_expression();
    match(T_SEMICOLON, "Expect ';' after return expression.");
    tree = modify_types(tree, function_id->type, 0);

    if (tree == NULL) {
        fprintf(stderr, "Incompatible type to return.\n");
        exit(1);
    }

    tree = make_unary_ast_node(A_RETURN, TYPE_NONE, tree, NULL, 0);

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

    trueAST = single_statement();

    // If there is an additional 'else' then skip this token
    // and parse conditional part
    if (token.token == T_ELSE) {
        scan(&token);
        falseAST = single_statement();
    }

    return make_ternary_astnode(A_IF, TYPE_NONE, condAST, trueAST, falseAST, NULL, 0);
}

t_astnode* while_statement(void) {
    t_astnode* condAST, *bodyAST;


    match(T_WHILE, "while");
    match(T_LEFT_PAREN, "(");

    condAST = binary_expression();

    if (condAST->op < A_EQUALS || condAST->op > A_GREATER_EQUAL) {
        fprintf(stderr, "Bad comparison operator\n");
    }

    match(T_RIGHT_PAREN, ")");


    loop_level++;
    bodyAST = single_statement();
    loop_level--;

    return make_astnode(A_WHILE, TYPE_NONE, condAST, bodyAST, NULL, 0);
}