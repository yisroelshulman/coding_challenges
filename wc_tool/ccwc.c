#include <stdio.h>
#include <stdbool.h>

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
    // EOF is not considered.
    char_count--;
    byte_count--;
    printf("bytes: %ld, lines: %ld, words: %ld, chars: %ld", byte_count, line_count, word_count, char_count);
}

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