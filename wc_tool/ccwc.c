#include <stdio.h>

typedef enum {
    WC_SUCCESS,
    WC_FAIL
} WCStatus;


WCStatus count_bytes(const char* file_name);
WCStatus count_lines(const char* file_name);
WCStatus count_words(const char* file_name);
WCStatus count_chars(const char* file_name);

int main(const int argc, const char* argv[]) {
    printf("args: %d\n", argc);

    WCStatus stat = WC_FAIL;

    if (argc == 2) {
        stat = count_chars(argv[1]);
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

WCStatus count_lines(const char* file_name) {

    FILE* fp =  fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", file_name);
        return WC_FAIL;
    }

    char c;
    long count = 0;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '\n') count++;
    }
    
    printf("%ld", count);
    return WC_SUCCESS;
}

WCStatus count_words(const char* file_name) {

    FILE* fp =  fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", file_name);
        return WC_FAIL;
    }

    char c = fgetc(fp);
    long count = 0;
    while(c == ' ' || c == '\n' || c == '\t' || c == '\r') {
        c = fgetc(fp);
    }

    while ((c = fgetc(fp)) != EOF) {
        if (c == ' ' || c == '\n' || c =='\t' || c == '\r') {
            count++;
            while(c == ' ' || c == '\n' || c == '\t' || c == '\r') {
                c = fgetc(fp);
                if (c == EOF) count--;
            }

        }
    }
    count++; // last word
    printf("%ld", count);
    return WC_SUCCESS;
}

WCStatus count_chars(const char* file_name) {

    FILE* fp =  fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Can't open %s\n", file_name);
        return WC_FAIL;
    }

    long count = 0;
    char c;


    while ((c = fgetc(fp)) != EOF) {
        count++;
    }
    printf("%ld", count);
    return WC_SUCCESS;
}