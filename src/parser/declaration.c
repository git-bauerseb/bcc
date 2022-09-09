#include "../../include/ast.h"

static t_symbol_entry* composite_declaration(int type) {
    t_symbol_entry* ctype = NULL;
    t_symbol_entry* m;

    int offset = 0;

    int com_type = token.token;

    scan(&token);

    if (com_type == T_STRUCT) {
        ctype = find_struct_symbol(text);
    } else {
        ctype = find_union_symbol(text);
    }

    scan(&token);


    // Struct is a type so return at this point
    if (token.token != T_LEFT_BRACE) {
        if (ctype == NULL) {
            report_error("struct_declaration(): Unknown structure type.\n");
        }

        return ctype;
    }

    // Here, a '{' bracket has been found so it is a struct declaration
    if (ctype) {
        report_error("struct_declaration(): struct named %s has already been defined.\n", ctype->name);
    }

    if (type == TYPE_STRUCT) {
        ctype = add_struct_symbol(text, TYPE_STRUCT, 0, NULL, 0);
    } else {
        ctype = add_union_symbol(text, TYPE_UNION, 0, NULL, 0);
    }

    scan(&token);

    var_declaration_list(NULL, C_MEMBER, T_SEMICOLON, T_RIGHT_BRACE);

    match(T_RIGHT_BRACE, "Expect '}' after variable declaration inside struct.");

    ctype->member = member_symbols->head;
    member_symbols->head = member_symbols->tail = NULL;

    m = ctype->member;
    m->offset = 0;

    offset = typesize(m->type, m->ctype);

    for (m = m->next; m != NULL; m = m->next) {
        if (type == TYPE_STRUCT) {
            m->offset = get_alignment(m->type, offset, 1);
        } else {
            m->offset = 0;
        }
        offset += typesize(m->type, m->ctype);
    }

    ctype->size = offset;
    return ctype;
}

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
        new_function = add_global_symbol(text, type, S_FUNCTION, NULL, 0, C_GLOBAL);
    }

    match(T_LEFT_PAREN, "(");
    p_count = var_declaration_list(old_function, C_PARAMETER, T_COMMA, T_RIGHT_PAREN);
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

int var_declaration_list(t_symbol_entry* function_symbol, int class, int separator, int end_token) {
    int type, p_count;
    p_count = 0;

    t_symbol_entry* function_prototype = NULL;
    t_symbol_entry* ctype;

    if (function_symbol != NULL) {
        function_prototype = function_symbol->member;
    }

    while (token.token != end_token) {
        type = parse_type(&ctype);
        match(T_IDENTIFIER, "Expected identifier");

        if (function_prototype != NULL) {
            if (type != function_prototype->type) {
                report_error("Type mismatch");
            }

            function_prototype = function_prototype->next;
        } else {
            var_declaration(type, ctype, class);
        }

        p_count++;

        if (token.token != separator && token.token != end_token) {
            report_error("var_declaration_list(): Unexpected token in parameter list.\n");
        }

        if (token.token == separator) {
            scan(&token);
        }
    }

    if (function_symbol != NULL && p_count != function_symbol->params) {
        report_error("parameter_declaration(): Parameter mismatch between function declaration and definition");
    }

    return p_count;
}

void global_declarations(void) {
    t_astnode* tree;
    t_symbol_entry* ctype;
    int type;

    while (1) {
        if (token.token == T_EOF) {
            break;
        }

        type = parse_type(&ctype);

        if ((type == TYPE_STRUCT || type == TYPE_UNION) && token.token == T_SEMICOLON) {
            scan(&token);
            continue;
        }

        match(T_IDENTIFIER, "identifier");

        if (token.token == T_LEFT_PAREN) {
            tree = function_declaration(type);

            if (tree == NULL) {
                continue;
            }

            print_ast(tree, 0);
            generate_ast(tree, NOREG, 0);
        } else {
            var_declaration(type, ctype, C_GLOBAL);       // Parse global declarations
            match(T_SEMICOLON, ";");
        }
    }
}

t_symbol_entry* var_declaration(int type, t_symbol_entry* ctype, int class) {

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
        case C_MEMBER:
            if (find_symbol_member(text) != NULL) {
                report_error("var_declaration(): Already defined struct member %s.\n", text);
            }
            break;
    }

    if (token.token == T_LEFT_BRACKET) {
    scan(&token);

    if (token.token == T_INTLIT) {
        switch (class) {
            case C_GLOBAL:
                symbol = add_global_symbol(text, pointer_to(type), S_ARRAY, ctype, token.value, class);
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
                symbol = add_global_symbol(text, type, S_VARIABLE, ctype, 1, class);
                break;
            case C_LOCAL:
                symbol = add_local_symbol(text, type, S_VARIABLE, ctype, 1, class);
                break;
            case C_PARAMETER:
                symbol = add_parameter_symbol(text, type, S_VARIABLE, ctype, 1, class);
                break;
            case C_MEMBER:
                symbol = add_member_symbol(text, type, S_VARIABLE, ctype, 1);
                break;
        }
    }

    return symbol;
}

int parse_type(t_symbol_entry** ctype) {
    int type;

    switch(token.token) {
        case T_VOID:
            type = TYPE_VOID;
            scan(&token);
            break;
        case T_CHAR:
            type = TYPE_CHAR;
            scan(&token);
            break;
        case T_INT:
            type = TYPE_INT;
            scan(&token);
            break;
        case T_LONG:
            type = TYPE_LONG;
            scan(&token);
            break;
        case T_STRUCT:
            type = TYPE_STRUCT;
            *ctype = composite_declaration(TYPE_STRUCT);
            break;
        case T_UNION:
            type = TYPE_UNION;
            *ctype = composite_declaration(TYPE_UNION);
        default:
            fprintf(stderr, "Illegal type, token %d\n", token.token);
    }

    while (1) {
        if (token.token != T_STAR) {break;}
        type = pointer_to(type);
        scan(&token);
    }

    return type;
}