#include <stdio.h>

typedef struct {
    char *scan;
    int x;
} Test;

Test test;

int main(const int argc, const char* argv[]) {
    printf("str %c\n", test.scan[0]);
    printf("test: %d\n", test.x);
    
    printf("Hello, World!\n");
}