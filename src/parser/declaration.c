#include "../../include/ast.h"


// Given a type, check if the current token is a literal of that type.
// If it is an integer literal, return the value.
// if it's a string literal, return the label number of the string.
static int parse_literal(int type) {
    if ((type == pointer_to(TYPE_CHAR)) && (token.token == T_STRINGLIT)) {
        return generate_global_string(text);
    }

    if (token.token == T_INTLIT) {
        switch (type) {
            case TYPE_CHAR:
                if (token.value < 0 || token.value > 255) {
                    report_error("parse_literal(): Value too big for char type.\n");
                }
            case TYPE_INT:
            case TYPE_LONG:
                break;
            default:
                report_error("parse_literal(): Type mismatch.\n");
        }
    } else {
        report_error("parse_literal(): Expecingt an integer literal value.\n");
    }

    return token.value;
}

static int parse_stars(int type) {
    while (1) {
        if (token.token != T_STAR) {break;}
        type = pointer_to(type);
        scan(&token);
    }

    return type;
}

static t_symbol_entry* array_declaration(
        char* varname,
        int type,
        t_symbol_entry* composite_type,
        int class) {

    int i = 0;
    int number_elements = 0;
    int max_elements = 0;
    int* init_list;
    t_symbol_entry* symbol = NULL;
    scan(&token);

    if (token.token == T_INTLIT) {

        if (token.value < 0) {
            report_error("array_declaration(): Size of array less than 0.\n");
        }

        number_elements = token.value;
        scan(&token);
    }

    match(T_RIGHT_BRACKET, "array_declaration(): Expect ']'");

    switch (class) {
        case C_EXTERN:
        case C_GLOBAL:
            symbol = add_global_symbol(varname, pointer_to(type), composite_type, S_ARRAY, class, 0, 0);
            break;
        default:
            report_error("array_declaration(): No-local array supported.\n");
    }

    if (token.token == T_ASSIGNMENT) {
        scan(&token);

        match(T_LEFT_BRACE, "Expect '{'");

        if (number_elements > 0) {max_elements = number_elements;}
        else {max_elements = 8;}

        init_list = malloc(sizeof(int)*max_elements);

        while (1) {
            if (number_elements > 0 && i == max_elements) {
                report_error("array_declaration(): Too many elements in array initializer list.\n");
            }

            if (number_elements <= 0 && i == max_elements) {
                init_list = realloc(init_list, max_elements * sizeof(int));
            }

            init_list[i] = parse_literal(type);
            i++;
            scan(&token);

            if (token.token == T_RIGHT_BRACE) {
                scan(&token);
                break;
            }

            match(T_COMMA, "Expect ','");
        }


        int j;
        for (j = i; j < symbol->num_elements; j++) {init_list[j] = 0;}

        symbol->initializer_list = init_list;
    }

    symbol->num_elements = number_elements;
    symbol->size = number_elements * typesize(type, composite_type);

    if (class == C_GLOBAL) {
        generate_global_symbol(symbol);
    }

    return symbol;
}

static t_symbol_entry* scalar_declaration(
        char* varname,
        int type,
        t_symbol_entry* composite_type,
        int class) {

    t_symbol_entry* symbol;

    switch (class) {
        case C_EXTERN:
        case C_GLOBAL:
            symbol = add_global_symbol(varname, type, composite_type, S_VARIABLE, class, 1, 0);
            break;
        case C_LOCAL:
            symbol = add_local_symbol(varname, type, composite_type, S_VARIABLE, 1);
            break;
        case C_PARAMETER:
            symbol = add_parameter_symbol(varname, type, composite_type, S_VARIABLE);
            break;
        case C_MEMBER:
            symbol = add_member_symbol(varname, type, composite_type, S_VARIABLE, 1);
            break;
    }

    if (token.token == T_ASSIGNMENT) {
        if (class != C_GLOBAL && class != C_LOCAL) {
            report_error("scalar_delcaration(): Variable %s cannot be initialized.\n", varname);
        }

        scan(&token);

        if (class == C_GLOBAL) {
            symbol->initializer_list = (int*)malloc(sizeof(int));
            symbol->initializer_list[0] = parse_literal(type);
            scan(&token);
        }
    }

    if (class == C_GLOBAL) {
        generate_global_symbol(symbol);
    }

    return symbol;
}

static void array_initialization(
        t_symbol_entry* symbol,
        int type,
        t_symbol_entry* composite_type,
        int class) {
    report_error("array_initialization():\n");
}

static t_symbol_entry* symbol_declaration(
        int type,
        t_symbol_entry* comp_type,
        int class
        ) {

    t_symbol_entry* symbol = NULL;
    char* varname = strdup(text);

    int stype = S_VARIABLE;

    match(T_IDENTIFIER, "symbol_declaration(): Expect identifier.\n");

    if (token.token == T_LEFT_PAREN) {
        symbol = function_declaration(varname, type, comp_type, class);
        return symbol;
    }

    switch (class) {
        case C_EXTERN:
        case C_GLOBAL:
            if (find_global_symbol(varname) != NULL) {
                report_error("symbol_declaration(): Global variable %s has already been declared.\n", varname);
            }
            break;
        case C_LOCAL:
        case C_PARAMETER:
            if (find_local_symbol(varname) != NULL) {
                report_error("symbol_declaration(): Local variable %s has already been declared.\n", varname);
            }
            break;
        case C_MEMBER:
            if (find_member_symbol(varname) != NULL) {
                report_error("symbol_declaration(): Memeber %s has already been declared.\n", varname);
            }
            break;
    }

    // Array declaration
    if (token.token == T_LEFT_BRACKET) {
        symbol = array_declaration(varname, type, comp_type, class);
        stype = S_ARRAY;
    } else {
        symbol = scalar_declaration(varname, type, comp_type, class);
    }

    return symbol;
}


int declaration_list(
        t_symbol_entry** comp_type,
        int class,
        int end1,
        int end2
        ) {

    int initial_type;
    int type;

    t_symbol_entry* symbol;

    if ((initial_type = parse_type(comp_type, &class)) == -1) {
        return initial_type;
    }

    while (1) {
        type = parse_stars(initial_type);
        symbol = symbol_declaration(type, *comp_type, class);

        if (symbol->stype == S_FUNCTION) {
            if (class != C_GLOBAL) {
                report_error("declaration_list(): Function definition not at global level.\n");
            }

            return type;
        }

        if (token.token == end1 || token.token == end2) {
            return type;
        }

        match(T_COMMA, "Expect ',' in declaration list.");
    }
}


static int parameter_declaration_list(
        t_symbol_entry* old_function_symbol,
        t_symbol_entry* new_function_symbol) {

    int type;
    int parameter_count = 0;

    t_symbol_entry* composite_type;
    t_symbol_entry* proto_ptr = NULL;

    if (old_function_symbol != NULL) {
        proto_ptr = old_function_symbol->member;
    }

    while (token.token != T_RIGHT_PAREN) {
        type = declaration_list(&composite_type, C_PARAMETER, T_COMMA, T_RIGHT_PAREN);

        if (type == -1) {report_error("parameter_declaration_list(): Bad type.\n");}

        if (proto_ptr != NULL) {
            if (type != proto_ptr->type) {
                report_error("parameter_declaration_list(): Type mismatch declaration <=> defintion.\n");
            }

            proto_ptr = proto_ptr->next;
        }

        parameter_count++;


        if (token.token == T_RIGHT_PAREN) {
            break;
        }

        match(T_COMMA, "Expect ','");
    }

    if (old_function_symbol != NULL && parameter_count != old_function_symbol->params) {
        report_error("parameter_declaration_list(): Parameter count mismatch.\n");
    }

    return parameter_count;
}

static int type_of_typedef(char* name, t_symbol_entry* ctype) {
    t_symbol_entry* t;

    if ((t = find_typedef_symbol(name)) == NULL) {
        report_error("type_of_typedef(): Unknown type %s\n", name);
    }

    scan(&token);
    ctype = t->ctype;
    return t->type;
}

static int typedef_declaration(t_symbol_entry** ctype) {
    int type;
    int class = C_TYPEDEF;

    // Skip 'typedef'
    scan(&token);

    type = parse_type(ctype, &class);

    if (find_typedef_symbol(text) != NULL) {
        report_error("typedef_declaration(): Redeclaration of typedef %s.\n", text);
    }

    type = parse_stars(type);

    scan(&token);
    add_typedef_symbol(text, type, *ctype);
    return type;
}

static void enum_declaration() {

    t_symbol_entry* enum_entry = NULL;
    char* name = NULL;
    int int_value;

    // Scan 'enum' keyword
    scan(&token);

    if (token.token == T_IDENTIFIER) {
        enum_entry = find_enum_symbol(text);
        name = strdup(text);
        scan(&token);
    }

    if (token.token != T_LEFT_BRACE) {
        if (enum_entry == NULL) {
            report_error("enum_declaration(): Expected enum type before %s\n", text);
        }

        return;
    }

    // Scan '{'
    scan(&token);

    if (enum_entry != NULL) {
        report_error("enum_declaration(): Enum %s has been redeclared.\n", enum_entry->name);
    } else {
        enum_entry = add_enum_symbol(name, C_ENUM_TYPE, 0);
    }

    // Parse identifiers inside enum
    while (1) {
        match(T_IDENTIFIER, "Expect identifier inside enum declaration.\n");

        name = strdup(text);

        enum_entry = find_enum_value(name);
        if (enum_entry != NULL) {
            report_error("enum_declaration(): Redeclaration of enum value %s.\n", name);
        }

        if (token.token == T_ASSIGNMENT) {
            scan(&token);
            if (token.token != T_INTLIT) {
                report_error("enum_declaration(): Only int literals allowed for enum initialization.\n");
            }

            int_value = token.value;
            scan(&token);
        } else {
            int_value++;
        }

        enum_entry = add_enum_symbol(name, C_ENUM_VALUE, int_value);

        if (token.token == T_RIGHT_BRACE) {
            break;
        }

        match(T_COMMA, "enum_declaration(): Expect comma after declaration.\n");
    }

    // Scan closing '}'
    scan(&token);
}

static t_symbol_entry* composite_declaration(int type) {
    t_symbol_entry* ctype = NULL;
    t_symbol_entry* m;

    int offset;
    int t;

    scan(&token);

    if (token.token == T_IDENTIFIER) {
        if (type == TYPE_STRUCT) {
            ctype = find_struct_symbol(text);
        } else {
            ctype = find_union_symbol(text);
        }

        scan(&token);
    }

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
        ctype = add_struct_symbol(text);
    } else {
        ctype = add_union_symbol(text);
    }

    scan(&token);


    while (1) {
        t = declaration_list(&m, C_MEMBER, T_SEMICOLON, T_RIGHT_BRACE);

        if (t == -1) {
            report_error("Bad type.\n");
        }

        if (token.token == T_SEMICOLON) {
            scan(&token);
        }

        if (token.token == T_RIGHT_BRACE) {
            break;
        }
    }

    match(T_RIGHT_BRACE, "Expect '}' after variable declaration inside struct.");

    if (member_symbols->head == NULL) {
        report_error("No members in struct.\n");
    }

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

t_symbol_entry * function_declaration(
        char* function_name,
        int type,
        t_symbol_entry* composite_type,
        int class) {
    t_astnode* tree, *finalstmt;
    int p_count;

    int endlabel;

    loop_level = 0;

    t_symbol_entry* old_function_symbol, *new_function_symbol;
    old_function_symbol = new_function_symbol = NULL;

    // Check if function prototype exists
    if ((old_function_symbol = find_symbol(function_name)) != NULL) {
        if (old_function_symbol->stype != S_FUNCTION) {
            old_function_symbol = NULL;
        }
    }

    // If the currently parsed function is a new function declaration
    // then add function to symbol table
    if (old_function_symbol == NULL) {
        endlabel = label();
        new_function_symbol = add_global_symbol(function_name, type, NULL, S_FUNCTION, C_GLOBAL, endlabel, 0);
    }

    match(T_LEFT_PAREN, "(");
    p_count = parameter_declaration_list(old_function_symbol, new_function_symbol);
    match(T_RIGHT_PAREN, ")");

    // Update parameters of function
    if (new_function_symbol) {
        new_function_symbol->params = p_count;
        new_function_symbol->member = parameter_symbols->head;
        old_function_symbol = new_function_symbol;
    }

    clear_parameter_symbols();

    if (token.token == T_SEMICOLON) {
        scan(&token);
        return old_function_symbol;
    }

    function_id = old_function_symbol;

    loop_level = 0;
    switch_level = 0;

    match(T_LEFT_BRACE, "Expect '{' before function body.");
    tree = compound_statement(0);
    match(T_RIGHT_BRACE, "Expect '}' after function body.");

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

    tree = make_unary_ast_node(A_FUNCTION, type, tree, old_function_symbol, endlabel);

    print_ast(tree, 1);

    generate_ast(tree, NOLABEL, NOLABEL, NOLABEL, 0);
    clear_local_symbol_table();

    return old_function_symbol;
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
        type = parse_type(&ctype, &class);
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
    t_symbol_entry* ctype;

    while (token.token != T_EOF) {
        declaration_list(&ctype, C_GLOBAL, T_SEMICOLON, T_EOF);
        if (token.token == T_SEMICOLON) {
            scan(&token);
        }
    }
}

t_symbol_entry* var_declaration(int type, t_symbol_entry* ctype, int class) {

    t_symbol_entry* symbol = NULL;

    switch (class) {
        case C_GLOBAL:
            if (find_global_symbol(text) != NULL) {
                report_error("var_declaration(): Already defined global variable %s.\n", text);
            }
            break;
        case C_LOCAL:
        case C_PARAMETER:
            if (find_local_symbol(text) != NULL) {
                report_error("var_declaration(): Already defined local variable %s.\n", text);
            }
            break;
        case C_MEMBER:
            if (find_member_symbol(text) != NULL) {
                report_error("var_declaration(): Already defined struct member %s.\n", text);
            }
            break;
    }

    if (token.token == T_LEFT_BRACKET) {
    scan(&token);

    if (token.token == T_INTLIT) {
        switch (class) {
            case C_GLOBAL:
            case C_EXTERN:
                symbol = add_global_symbol(text, pointer_to(type), ctype, S_ARRAY, class, token.value, 0);
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
                symbol = add_global_symbol(text, type, ctype, S_VARIABLE, class, 1, 0);
                break;
            case C_LOCAL:
                symbol = add_local_symbol(text, type, ctype, S_VARIABLE, 1);
                break;
            case C_PARAMETER:
                symbol = add_parameter_symbol(text, type, ctype, S_VARIABLE);
                break;
            case C_MEMBER:
                symbol = add_member_symbol(text, type, ctype, S_VARIABLE, 1);
                break;
        }
    }

    return symbol;
}

int parse_type(t_symbol_entry** ctype, int *class) {
    int type;
    int modifier = 1;
    
    while (modifier) {
        switch (token.token) {
            case T_EXTERN:
                *class = C_EXTERN;
                // Consume 'extern'
                scan(&token);
                break;
            default:
                modifier = 0;
        }
    }

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
            if (token.token == T_SEMICOLON) {
                return -1;
            }
            break;
        case T_UNION:
            type = TYPE_UNION;
            *ctype = composite_declaration(TYPE_UNION);
            if (token.token == T_SEMICOLON) {
                return -1;
            }
            break;
        case T_ENUM:
            type = TYPE_INT;
            enum_declaration();
            if (token.token == T_SEMICOLON) {
                return -1;
            }
            break;
        case T_TYPEDEF:
            type = typedef_declaration(ctype);
            if (token.token == T_SEMICOLON) {
                return -1;
            }
            break;
        case T_IDENTIFIER:
            type = type_of_typedef(text, *ctype);
            break;
        default:

            fprintf(stderr, "Illegal type, token %d\n", token.token);
    }

    return type;
}