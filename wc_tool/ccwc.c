#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// bit flags
#define WC_INVALID_FLAG 0u
#define WC_BYTES        1u << 0
#define WC_LINE         1u << 1
#define WC_WORD         1u << 2
#define WC_CHAR         1u << 3
#define WC_UNSET        1u << 4

typedef struct {
    int byte_count;
    int line_count;
    int word_count;
    int char_count;
    FILE* input_stream;
} WCInfo;

// assigning stream is safe because it is either stdin or was checked for NULL before the call to
// this function
static WCInfo init_wc_info(FILE* stream) {
    WCInfo file = {
        .byte_count = 0,
        .line_count = 0,
        .char_count = 0,
        .word_count = 0,
        .input_stream = stream
    };
    return file;
}

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

static char consume_whitespaces(WCInfo* file) {
    char c = next(file);
    while (is_whitespace(c)) c = next(file);
    return c;
}

static void wc_default(WCInfo* file) {
    char c = consume_whitespaces(file);
    while (c != EOF) {
        if (is_whitespace(c)) {
            file->word_count++;
            c = consume_whitespaces(file);
        } else {
            c = next(file);
        }
    }
    // EOF was added in next(), and is not considered.
    file->char_count--;
    file->byte_count--;
}

static uint8_t set_flags(uint8_t flag, const char* flags) {
    if (flag&WC_UNSET) flag = 0;
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

static void print_result(const WCInfo* file, const uint8_t flags) {
    if (flags&WC_LINE) {
        printf("\t%d", file->line_count);
    }
    if (flags&WC_WORD) {
        printf("\t%d", file->word_count);
    }
    if (flags&WC_BYTES) {
        printf("\t%d", file->byte_count);
    }
    if (flags&WC_CHAR) {
        printf("\t%d", file->char_count);
    }
    printf("\n");
}

int main(const int argc, const char* argv[]) {

    uint8_t flags = WC_UNSET;
    uint8_t num_files = 0; // arbitraty max num files to 256

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            flags = set_flags(flags, argv[i]);
            if (flags == WC_INVALID_FLAG) {
                printf("invalid flag [%s]\n", argv[i]);
                return 1;
            }
        } else {
            num_files++;
            FILE* stream = fopen(argv[i], "r");
            if (stream == NULL) {
                printf("[%s] No such file or directory.\n", argv[i]);
            } else {
                WCInfo file = init_wc_info(stream);
                wc_default(&file);
                file.input_stream = NULL;
                fclose(stream);
            }
        }
    }

    if (num_files == 0) {
        if (flags&WC_UNSET) flags = 0b111; // setting the proper bytes to 1
        WCInfo file = init_wc_info(stdin);
        wc_default(&file);
        print_result(&file, flags);
    }
}