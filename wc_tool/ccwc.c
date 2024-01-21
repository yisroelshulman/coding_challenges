#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*the flags to determine what should get printed*/
#define WC_INVALID_FLAG 0u
#define WC_BYTE         1u << 0
#define WC_LINE         1u << 1
#define WC_WORD         1u << 2
#define WC_CHAR         1u << 3
#define WC_UNSET        1u << 4
#define WC_DEFAULT      0b111

/*the struct that holds the files info*/
typedef struct
{
    bool error;
    unsigned int line_count;
    unsigned int word_count;
    unsigned int char_count;
    unsigned int byte_count;
} WCFile;

/*the struct for the totals*/
typedef struct
{
    unsigned long line_count;
    unsigned long word_count;
    unsigned long char_count;
    unsigned long byte_count;
} WCTotal;

static bool is_whitespace(const char c)
{
    return c == ' ' || c == '\n'  || c == '\t' || c == '\r';
}

static WCFile wc_file(FILE* input_stream)
{
    WCFile file = {false, 0, 0, 0, 0};
    bool in_word = false;
    char c = fgetc(input_stream);
    while (c != EOF)
    {
        file.byte_count++;
        file.char_count++;
        if (c == '\n') { file.line_count++; }
        if (is_whitespace(c))
        {
            if (in_word)
            {
                file.word_count++;
                in_word = false;
            }
        }
        else { in_word = true; }

        c = fgetc(input_stream);
    }
    return file;
}

static uint8_t set_wc_flags(const uint8_t flag, const char* flags)
{
    uint8_t new_flag = flag;
    int pos = 1;
    char c = flags[pos++]; //first char is '-' so we start at index 1
    while (c != '\0')
    {
        switch (c)
        {
            case 'c': new_flag |= WC_BYTE; break;
            case 'm': new_flag |= WC_CHAR; break;
            case 'l': new_flag |= WC_LINE; break;
            case 'w': new_flag |= WC_WORD; break;
            default: return WC_INVALID_FLAG;
        }
        c = flags[pos++];
    }
    return new_flag;
}

static void print_wc_result(const WCFile* file, const uint8_t flags, const char* file_name)
{
    if (flags&WC_LINE) { printf("%7d ", file->line_count); }
    if (flags&WC_WORD) { printf("%7d ", file->word_count); }
    if (flags&WC_CHAR) { printf("%7d ", file->char_count); }
    if (flags&WC_BYTE) { printf("%7d ", file->byte_count); }
    printf("%s\n", file_name);
}

static void print_wc_totals(const WCTotal* totals, const uint8_t flags)
{
    if (flags&WC_LINE) { printf("%7lu ", totals->line_count); }
    if (flags&WC_WORD) { printf("%7lu ", totals->word_count); }
    if (flags&WC_BYTE) { printf("%7lu ", totals->byte_count); }
    if (flags&WC_CHAR) { printf("%7lu ", totals->char_count); }
    printf("total\n");
}

static uint8_t min(const int x, const int y)
{
    if (x < y) return x;
    return y;
}

int main(const int argc, const char* argv[])
{
    bool had_error = false;
    uint8_t flags = WC_UNSET;
    uint8_t num_files = 0; // arbitraty max num files to 256
    uint8_t* indices = malloc(sizeof(uint8_t) * min(256, argc));
    WCFile* files = malloc(sizeof(WCFile) * min(256, argc));
    if (indices == NULL || files == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }
    WCTotal totals = {0, 0, 0, 0};

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            if (flags&WC_UNSET) { flags = 0; }
            flags = set_wc_flags(flags, argv[i]);
            if (flags == WC_INVALID_FLAG)
            {
                printf("invalid flag [%s]\n", argv[i]);
                return 1;
            }
        }
        else
        {
            if (num_files + 1 == 256)
            {
                printf("Too many files\n");
                return 1;
            }
            indices[num_files] = i;
            FILE* stream = fopen(argv[i], "r");
            if (stream == NULL)
            {
                had_error = true;
                files[num_files] = (WCFile){true, 0, 0, 0, 0};
            }
            else
            {
                files[num_files] = wc_file(stream);
                totals.byte_count += files[num_files].byte_count;
                totals.line_count += files[num_files].line_count;
                totals.word_count += files[num_files].word_count;
                totals.char_count += files[num_files].char_count;
                fclose(stream);
            }
            num_files++;
        }
    }

    if (flags&WC_UNSET) { flags = WC_DEFAULT; } // setting the proper bytes to 1 for the default setting

    switch (num_files)
    {
        case 0:
        {
            WCFile file = wc_file(stdin);
            print_wc_result(&file, flags, "");
            break;
        }
        case 1:
        {
            if (files[0].error) { printf("[%s] No such file or directory.\n", argv[indices[0]]); }
            else { print_wc_result(&files[0], flags, argv[indices[0]]); }
            break;
        }
        default:
        {
            for (int i = 0; i < num_files; i++)
            {
                if (files[i].error) { printf("[%s] No such file or directory.\n", argv[indices[i]]); }
                else { print_wc_result(&files[i], flags, argv[indices[i]]); }
            }
            print_wc_totals(&totals, flags);
            break;
        }
    }

    free(indices); // not needed
    free(files); // not needed
    if (had_error) return 1;
    return 0;
}