#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// bit flags
#define WC_INVALID_FLAG 0u
#define WC_BYTES        1u << 0
#define WC_LINE         1u << 1
#define WC_WORD         1u << 2
#define WC_CHAR         1u << 3

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

static char consume_whitespace() {
    char c = next();
    while (is_whitespace(c)) c = next();
    return c;
}

static void wc_default() {

    char c = consume_whitespace();
    while (c != EOF) {
        if (is_whitespace(c)) {
            word_count++;
            c = consume_whitespace();
        } else {
            c = next();
        }
    }
    // EOF was added in next(), and is not considered.
    char_count--;
    byte_count--;
    printf("bytes: %ld, lines: %ld, words: %ld, chars: %ld", byte_count, line_count, word_count, char_count);
}

static uint8_t set_flags(const uint8_t flag, const char* flags) {
    uint8_t current_flag = 0;
    int pos = 1;
    char c = flags[pos++]; // first char
    while (c != '\0') {
        switch (c) {
            case 'c': current_flag = current_flag|WC_BYTES; break;
            case 'm': current_flag = current_flag|WC_CHAR;  break;
            case 'l': current_flag = current_flag|WC_LINE;  break;
            case 'w': current_flag = current_flag|WC_WORD;  break;
            default:
                return WC_INVALID_FLAG;
        }
        c = flags[pos++];
    }
    return flag|current_flag;
}

static void print_flags(const uint8_t flag) {
    if (flag&WC_BYTES) {
        printf("byte set ");
    }
    if (flag&WC_LINE) {
        printf("line set ");
    }
    if (flag&WC_WORD) {
        printf("word set ");
    }
    if (flag&WC_CHAR) {
        printf("char set ");
    }
    printf("\n");
}

int main(const int argc, const char* argv[]) {

    uint8_t flag = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            flag = set_flags(flag, argv[i]);
            if (flag == WC_INVALID_FLAG) {
                printf("invalid flag [%s]\n", argv[i]);
                return 1;
            }
        }
    }
    print_flags(flag);

    if (argc == 2) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            // error
            return 1;
        }
    } else {
        input_stream = stdin;
    }

    //wc_default();
    printf(" [%s]\n", argv[1]);

}