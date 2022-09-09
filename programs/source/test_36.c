int printf(char *fmt);

struct useless_point3d {
    int x;
    char y;
    long z;
};

struct useless_point3d p1;
struct useless_point3d* p2;

int main() {
    long result;

    p1.x = 123;
    printf("%d\n", p1.x);

    p1.y = 'c';
    printf("%d\n", p1.y);

    p1.z = 4005;
    printf("%d\n", p1.z);

    result = p1.x + p1.y + p1.z;
    printf("%d\n", result);

    p2 = &p1;
    result = p2->x + p2->y + p2->z;
    printf("%d\n", result);

    return 0;
}