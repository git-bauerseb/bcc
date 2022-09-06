#include "../../include/ast.h"

t_astnode* function_declaration(int type) {
    t_astnode* tree, *finalstmt;
    int nameslot;
    int paramcnt;

    int endlabel;
    int id;


    // Check if function prototype exists
    if (((id = find_symbol(text))) != -1) {
        if (sym_table[id].stype != S_FUNCTION) {
            id = -1;
        }
    }

    if (id == -1) {
        endlabel = label();
        nameslot = add_global(text, type, S_FUNCTION, endlabel, 0, C_GLOBAL);
    }

    match(T_LEFT_PAREN, "(");
    paramcnt = parameter_declaration(id);
    match(T_RIGHT_PAREN, ")");

    if (id == -1) {
        sym_table[nameslot].num_elements = paramcnt;
    }

    if (token.token == T_SEMICOLON) {
        scan(&token);
        return NULL;
    }

    if (id == -1) {
        id = nameslot;
    }
    copy_function_parameters(id);

    current_function_id = id;

    tree = compound_statement();

    // If the type is not void then force a return statement
    if (type != TYPE_VOID) {
        finalstmt = (tree->op == A_GLUE) ? tree->right : tree;

        if (finalstmt == NULL || finalstmt->op != A_RETURN) {
            fprintf(stderr, "No return for function with non-void type.\n");
        }
    }

    return make_ast_unary(A_FUNCTION, type, tree, id);
}

int parameter_declaration(int id) {
    int type, original_parameter_count;
    int num_params = 0;

    int parameter_id = id + 1;

    if (parameter_id) {
        original_parameter_count = sym_table[id].num_elements;
    }



    while (token.token != T_RIGHT_PAREN) {
        type = parse_type();
        match(T_IDENTIFIER, "Expected identifier");

        if (parameter_id) {
            if (type != sym_table[id].type) {
                report_error("Type mismatch");
            }
        } else {
            var_declaration(type, C_PARAMETER);
        }

        num_params++;

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

    if ((id != -1) && (num_params != original_parameter_count)) {
        report_error("Parameter mismatch");
    }

    return num_params;
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

void var_declaration(int type, int class) {

    // Text now has the identifier's name.
    // If the next token is a '['
    if (token.token == T_LEFT_BRACKET) {
    // Skip past the '['
    scan(&token);

    if (token.token == T_INTLIT) {
        if (class == C_LOCAL) {
            report_error("No local arrays.");
            // id = add_local(text, pointer_to(type), S_ARRAY, 0, token.value, isparam);
        } else {
            add_global(text, pointer_to(type), S_ARRAY, 0, token.value, class);
        }
    }

    // Ensure we have a following ']'
    scan(&token);
    match(T_RIGHT_BRACKET, "]");
    } else {
        // Add this as a known scalar
        // and generate its space in assembly
        if (class == C_LOCAL) {

            if (add_local(text, type, S_VARIABLE, 1, class) == -1) {
                report_error("Duplicate variable");
            }
        } else {
            add_global(text, type, S_VARIABLE, 0, 1, class);
        }
    }
}