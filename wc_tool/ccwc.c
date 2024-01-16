#include <stdio.h>
#include <stdbool.h>

typedef enum {
    WC_SUCCESS,
    WC_FAIL
} WCStatus;

FILE* input_stream;

static long byte_count = 0;
static long line_count = 0;
static long word_count = 0;
static long char_count = 0;


static char next() {
    char c = fgetc(input_stream);
    if (c == '\n') line_count ++;
    byte_count++;
    char_count++;
    return c;
}

static bool is_whitespace(const char c) {
    return c == ' ' || c == '\n'  || c == '\t' || c == '\r';
}

static char consume_white_space() {
    char c = next();
    while (is_whitespace(c)) c = next();
    return c;
}

static void wc_default() {

    char c = consume_white_space();
    while (c != EOF) {
        if (is_whitespace(c)) {
            word_count++;
            c = consume_white_space();
        } else {
            c = next();
        }
    }
    // EOF is not considered.
    char_count--;
    byte_count--;
    printf("bytes: %ld, lines: %ld, words: %ld, chars: %ld", byte_count, line_count, word_count, char_count);
}

WCStatus count_bytes(const char* file_name);
WCStatus count_lines(const char* file_name);
WCStatus count_words(const char* file_name);
WCStatus count_chars(const char* file_name);

int main(const int argc, const char* argv[]) {
    printf("args: %d\n", argc);

   

    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            // error
            return 1;
        }
    } else {
        input_stream = stdin;
    }


    wc_default();
    printf(" [%s]\n", argv[1]);

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