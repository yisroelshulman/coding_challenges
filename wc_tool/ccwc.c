#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// bit flags
#define WC_INVALID_FLAG 0u
#define WC_BYTES        1u << 0
#define WC_LINE         1u << 1
#define WC_WORD         1u << 2
#define WC_CHAR         1u << 3
#define WC_UNSET        1u << 4

// totals are global
static unsigned long total_bytes = 0;
static unsigned long total_lines = 0;
static unsigned long total_words = 0;
static unsigned long total_chars = 0;

typedef struct {
    bool error;
    unsigned int byte_count;
    unsigned int line_count;
    unsigned int word_count;
    unsigned int char_count;
    FILE* input_stream;
} WCInfo;

// assigning stream is safe because it is either stdin or was checked for NULL before the call to
// this function
static WCInfo init_wc_info(FILE* stream, const bool error) {
    WCInfo file = {
        .error = error,
        .byte_count = 0,
        .line_count = 0,
        .char_count = 0,
        .word_count = 0,
        .input_stream = stream
    };
    return file;
}

static char wc_next(WCInfo* file) {
    char c = fgetc(file->input_stream);
    if (c == '\n') {
         file->line_count ++;
         total_lines++;
    }
    file->byte_count++;
    total_bytes++;
    file->char_count++;
    total_chars++;
    return c;
}

static bool is_whitespace(const char c) {
    return c == ' ' || c == '\n'  || c == '\t' || c == '\r';
}

static char consume_wc_whitespaces(WCInfo* file) {
    char c = wc_next(file);
    while (is_whitespace(c)) c = wc_next(file);
    return c;
}

static void wc_counter(WCInfo* file) {
    char c = consume_wc_whitespaces(file);
    while (c != EOF) {
        if (is_whitespace(c)) {
            file->word_count++;
            total_words++;
            c = consume_wc_whitespaces(file);
        } else {
            c = wc_next(file);
        }
    }
    // EOF was added in next(), and is not considered.
    file->char_count--;
    total_chars--;
    file->byte_count--;
    total_bytes--;
}

static uint8_t set_wc_flags(uint8_t flag, const char* flags) {
    if (flag&WC_UNSET) flag = 0;
    uint8_t current_flag = 0;
    int pos = 1;
    char c = flags[pos++]; //first char is - so we start at index 1
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

static void print_wc_result(const WCInfo* file, const uint8_t flags, const char* file_name) {
    if (flags&WC_LINE) {
        printf("%8d ", file->line_count);
    }
    if (flags&WC_WORD) {
        printf("%8d ", file->word_count);
    }
    if (flags&WC_BYTES) {
        printf("%8d ", file->byte_count);
    }
    if (flags&WC_CHAR) {
        printf("%8d ", file->char_count);
    }
    printf("%s\n", file_name);
}

static void print_wc_totals(const uint8_t flags) {
    if (flags&WC_LINE) {
        printf("%8lu ", total_lines);
    }
    if (flags&WC_WORD) {
        printf("%8lu ", total_words);
    }
    if (flags&WC_BYTES) {
        printf("%8lu ", total_bytes);
    }
    if (flags&WC_CHAR) {
        printf("%8lu ", total_chars);
    }
    printf("total\n");
}

static uint8_t min(const int x, const int y) {
    if (x < y) return x;
    return y;
}

int main(const int argc, const char* argv[]) {
    bool had_error = false;
    uint8_t flags = WC_UNSET;
    uint8_t num_files = 0; // arbitraty max num files to 256
    uint8_t* indices = malloc(sizeof(uint8_t) * min(256, argc));
    WCInfo* files = malloc(sizeof(WCInfo) * min(256, argc));

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            flags = set_wc_flags(flags, argv[i]);
            if (flags == WC_INVALID_FLAG) {
                printf("invalid flag [%s]\n", argv[i]);
                return 1;
            }
        } else {
            if (num_files + 1 == 256) {
                printf("Too many files\n");
                return 1;
            }
            indices[num_files] = i;
            FILE* stream = fopen(argv[i], "r");
            if (stream == NULL) {
                had_error = true;
                files[num_files] = init_wc_info(NULL, true);
            } else {
                files[num_files] = init_wc_info(stream, false);
                wc_counter(&files[num_files]);
                files[num_files].input_stream = NULL; // overcautious
                fclose(stream);
            }
            num_files++;
        }
    }

    if (flags&WC_UNSET) flags = 0b111; // setting the proper bytes to 1 for the default setting

    switch (num_files) {
        case 0: {
            WCInfo file = init_wc_info(stdin, false);
            wc_counter(&file);
            print_wc_result(&file, flags, "");
            break;
        }
        case 1: {
            if (files[0].error) {
                printf("[%s] No such file or directory.\n", argv[indices[0]]);
            } else {
                print_wc_result(&files[0], flags, argv[indices[0]]);
            }
            break;
        }
        default: {
            for (int i = 0; i < num_files; i++) {
                if (files[i].error) {
                    printf("[%s] No such file or directory.\n", argv[indices[i]]);
                } else {
                    print_wc_result(&files[i], flags, argv[indices[i]]);
                }
            }
            print_wc_totals(flags);
            break;
        }
    }

    free(indices); // not needed
    free(files); // not needed
    if (had_error) return 1;
    return 0;
}