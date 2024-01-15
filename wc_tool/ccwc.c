#include <stdio.h>


typedef enum {
    WC_SUCCESS,
    WC_FAIL
} WCStatus;

WCStatus count_bytes(const char* file_name);

int main(const int argc, const char* argv[]) {
    printf("args: %d\n", argc);

    WCStatus stat = WC_FAIL;

    if (argc == 2) {
        stat = count_bytes(argv[1]);
        if (stat == WC_SUCCESS) {
            printf(" %s\n", argv[1]);
        }
    }
}

WCStatus count_bytes(const char* file_name) {

    FILE* fp =  fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", file_name);
        return WC_FAIL;
    } 

    if (fseek(fp, 0L, SEEK_END) != 0) {
        fprintf(stderr, "error: fseek\n");
        return WC_FAIL;
    }

    long bytes = ftell(fp);
    if (bytes == -1) {
        fprintf(stderr, "error: ftell\n");
        return WC_FAIL;
    }

    printf("%ld", bytes);
    return WC_SUCCESS;
}