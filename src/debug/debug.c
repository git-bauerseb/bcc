#include "../../include/ast.h"

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
    "A_RETURN", "A_ADDR", "A_DEREFERENCE", "A_STRLIT", "A_LSHIFT",
    "A_RSHIFT", "A_OR", "A_AND", "A_LOGIC_NOT",
    "A_POST_DECREMENT", "A_POST_INCREMENT", "A_NEGATE",
    "A_PRE_INCREMENT", "A_PRE_DECREMENT",
    "A_INVERT", "A_XOR"
};

void print_ast(t_astnode* root, int depth) {

    if (root == NULL) {return;}

    char prefBuff[depth+1];

    for (int i = 0; i < depth; i++) {prefBuff[i]=' ';}
    prefBuff[depth] = '\0';

    char* ast_name = ast_names[root->op-1];

    switch (root->op) {
        case A_ASSIGN: printf("%s%s\n", prefBuff, ast_name); break;
        case A_IDENTIFIER: printf("%s%s(%s) %s\n", prefBuff, ast_name, sym_table[root->v.id].name, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_INTLIT: printf("%s%s(%d) %s\n", prefBuff, ast_name, root->v.value, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_FUNCTION: printf("%s%s\n", prefBuff, ast_name); break;
        case A_GLUE: printf("%s%s\n", prefBuff, ast_name); break;
        case A_RETURN: printf("%s%s\n", prefBuff, ast_name); break;
        case A_DEREFERENCE: printf("%s%s %s\n", prefBuff, ast_name, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_FUNCTION_CALL: printf("%s%s %s\n", prefBuff, ast_name, root->rvalue != 0 ? "rvalue" : ""); break;
        case A_POST_DECREMENT: printf("%s%s \n", prefBuff, "POST_DECREMENT");
    }

    if (root->left != NULL) {
        print_ast(root->left, root->op == A_WIDEN ? depth : depth+2);
    }

    if (root->right != NULL) {
        print_ast(root->right, root->op == A_WIDEN ? depth : depth+2);
    }
}