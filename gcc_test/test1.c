#include <stdio.h>

int main() {
    int c;
    int *e;

    c = 42;
    e = &c + 1;

    printf("%d\n", *(e-1));
}