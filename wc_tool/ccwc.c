#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// bit flags
#define WC_INVALID_FLAG 0u
#define WC_BYTES        1u << 0
#define WC_LINE         1u << 1
#define WC_WORD         1u << 2
#define WC_CHAR         1u << 3

typedef struct {
    int byte_count;
    int line_count;
    int word_count;
    int char_count;
    FILE* input_stream;
} WCInfo;

static char next(WCInfo* file) {
    char c = fgetc(file->input_stream);
    if (c == '\n') file->line_count ++;
    file->byte_count++;
    file->char_count++;
    return c;
}

static bool is_whitespace(const char c) {
    return c == ' ' || c == '\n'  || c == '\t' || c == '\r';
}

static char consume_whitespace(WCInfo* file) {
    char c = next(file);
    while (is_whitespace(c)) c = next(file);
    return c;
}

static void wc_default(WCInfo* file) {
    char c = consume_whitespace(file);
    while (c != EOF) {
        if (is_whitespace(c)) {
            file->word_count++;
            c = consume_whitespace(file);
        } else {
            c = next(file);
        }
    }
    // EOF was added in next(), and is not considered.
    file->char_count--;
    file->byte_count--;
    printf("bytes: %d, lines: %d, words: %d, chars: %d\n", file->byte_count, file->line_count, file->word_count, file->char_count);
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
        } else {
            WCInfo file = {
                .byte_count = 0,
                .line_count = 0,
                .word_count = 0,
                .char_count = 0,
                .input_stream = fopen(argv[i], "r")
            };
            if (file.input_stream == NULL) {
                printf("[%s] No such file or directory\n", argv[i]);
            } else {
                wc_default(&file);
            }
        }
    }
    print_flags(flag);

    
    //wc_default();
    printf(" [%s]\n", argv[1]);

}