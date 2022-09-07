#include "../../include/ast.h"

t_astnode* function_declaration(int type) {
    t_astnode* tree, *finalstmt;
    int p_count;

    int endlabel;

    t_symbol_entry* old_function, *new_function;
    old_function = new_function = NULL;

    // Check if function prototype exists
    if ((old_function = find_symbol(text)) != NULL) {
        if (old_function->stype != S_FUNCTION) {
            old_function = NULL;
        }
    }

    // If the currently parsed function is a new function declaration
    // then add function to symbol table
    if (old_function == NULL) {
        endlabel = label();
        new_function = add_global_symbol(text, type, S_FUNCTION, C_GLOBAL, endlabel);
    }

    match(T_LEFT_PAREN, "(");
    p_count = parameter_declaration(old_function);
    match(T_RIGHT_PAREN, ")");

    // Update parameters of function
    if (new_function) {
        new_function->params = p_count;
        new_function->member = parameter_symbols->head;
        old_function = new_function;
    }

    clear_parameter_symbols();

    if (token.token == T_SEMICOLON) {
        scan(&token);
        return NULL;
    }

    function_id = old_function;

    tree = compound_statement();

    // If the type is not void then force a return statement
    if (type != TYPE_VOID) {

        if (tree == NULL) {
            report_error("function_declaration(): Function returning non-void has no statements.\n");
        }

        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;

        if (finalstmt == NULL || finalstmt->op != A_RETURN) {
            fprintf(stderr, "No return for function with non-void type.\n");
        }
    }

    return make_ast_unary(A_FUNCTION, type, tree, old_function, endlabel);
}

int parameter_declaration(t_symbol_entry* function_symbol) {
    int type, p_count;
    p_count = 0;

    t_symbol_entry* function_prototype = NULL;

    if (function_symbol != NULL) {
        function_prototype = function_symbol;
    }

    while (token.token != T_RIGHT_PAREN) {
        type = parse_type();
        match(T_IDENTIFIER, "Expected identifier");

        if (function_prototype != NULL) {
            if (type != function_prototype->type) {
                report_error("Type mismatch");
            }

            function_prototype = function_prototype->next;
        } else {
            var_declaration(type, C_PARAMETER);
        }

        p_count++;

        switch (token.token) {
            case T_COMMA:
                scan(&token);
                break;
            case T_RIGHT_PAREN:
                break;    
            default:
                report_error("Unexpected token in paramter list");
        }
    }

    if (function_symbol != NULL && p_count != function_symbol->params) {
        report_error("parameter_declaration(): Parameter mismatch between function declaration and definition");
    }

    return p_count;
}

void global_declarations(void) {
    t_astnode* tree;
    int type;

    while (1) {
        type = parse_type();
        match(T_IDENTIFIER, "identifier");

        if (token.token == T_LEFT_PAREN) {
            tree = function_declaration(type);
            generate_ast(tree, NOREG, 0);
            print_ast(tree, 0);
        } else {
            var_declaration(type, C_GLOBAL);       // Parse global declarations
            match(T_SEMICOLON, ";");
        }

        if (token.token == T_EOF) {
            break;
        }
    }
}

t_symbol_entry* var_declaration(int type, int class) {

    t_symbol_entry* symbol = NULL;

    switch (class) {
        case C_GLOBAL:
            if (find_symbol_global(text) != NULL) {
                report_error("var_declaration(): Already defined global variable %s.\n", text);
            }
            break;
        case C_LOCAL:
        case C_PARAMETER:
            if (find_symbol_local(text) != NULL) {
                report_error("var_declaration(): Already defined local variable %s.\n", text);
            }
            break;
    }

    if (token.token == T_LEFT_BRACKET) {
    scan(&token);

    if (token.token == T_INTLIT) {
        switch (class) {
            case C_GLOBAL:
                symbol = add_global_symbol(text, pointer_to(type), S_ARRAY, token.value, class);
                break;
            case C_LOCAL:
            case C_PARAMETER:
                report_error("var_declaration(): Local array definition not implemented.\n");
        }
    }

    scan(&token);
    match(T_RIGHT_BRACKET, "]");
    } else {
        switch (class) {
            case C_GLOBAL:
                symbol = add_global_symbol(text, type, S_VARIABLE, 1, class);
                break;
            case C_LOCAL:
                symbol = add_local_symbol(text, type, S_VARIABLE, 1, class);
                break;
            case C_PARAMETER:
                symbol = add_parameter_symbol(text, type, S_VARIABLE, 1, class);
                break;
        }
    }

    return symbol;
}