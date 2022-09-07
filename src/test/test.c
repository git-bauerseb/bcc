#include "../include/test.h"

void test_types() {
    int c_type = TYPE_CHAR;                     // char
    int pc_type = pointer_to(c_type);           // char*
    int ppc_type = pointer_to(pc_type);         // char**
    int pppc_type = pointer_to(ppc_type);       // char***

    char* str = get_type_representation(c_type);
    printf("%s\n", str);
    free(str);

    str = get_type_representation(pc_type);
    printf("%s\n", str);
    free(str);

    str = get_type_representation(ppc_type);
    printf("%s\n", str);
    free(str);

    str = get_type_representation(pppc_type);
    printf("%s\n", str);
    free(str);
}

void report_test_failed(const char* msg, ...) {
    va_list list;
    va_start(list, msg);

    char buffer[TEST_MSG_LENGTH];

    snprintf(buffer, TEST_MSG_LENGTH, "[TEST ERROR]:: %s", msg);
    fprintf(stderr, buffer, list);
    va_end(list);
}