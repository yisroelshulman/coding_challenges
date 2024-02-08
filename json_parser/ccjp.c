#include <stdio.h>
#include <stdlib.h>

#include "json_scanner.h"

static char* read_doc(char *source)
{
    FILE* file = fopen(source, "r");
    if (file == NULL)
    {
        fprintf(stderr, "[%s] Could not open file.\n", source);
        return NULL;
    }

    fseek(file, 0L, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(filesize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "[%s] Not enough memory to read file.", source);
        return NULL;
    }

    size_t bytesread = fread(buffer, sizeof(char), filesize, file);
    if (bytesread != filesize)
    {
        fprintf(stderr, "[%s] Could not read file.", source);
        return NULL;
    }

    fclose(file);
    buffer[filesize] = '\0';
    return buffer;
}

static void print_token(const Token token);

int main(const int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "No test file entered.\n");
        return 1;
    }
     char *source = read_doc(argv[1]);
     if (source == NULL)
     {
        // error already reported
        return 1;
     }

    init_scanner(source);
    Token token = scan_token();
    while (token.type != TOKEN_EOF)
    {
        print_token(token);
        token = scan_token();
    }
    print_token(token);
    return 0;
}

static void print_full_token(const Token token)
{
    switch (token.type)
    {
        case TOKEN_STRING: printf("token: [string]"); break;
        case TOKEN_NUMBER: printf("token: [number]"); break;
        case TOKEN_TRUE: printf("token: [true]"); break;
        case TOKEN_FALSE: printf("token: [false]"); break;
        case TOKEN_NULL: printf("token: [null]"); break;
        case TOKEN_ERROR: printf("token: [error]"); break;
        default: break;
    }
    printf("\n");
}

static void print_token(const Token token)
{
    switch (token.type)
    {
        case TOKEN_BEGIN_OBJECT: printf("token: [begin-obj]\n"); return;
        case TOKEN_END_OBJECT: printf("token: [end-obj]\n"); return;
        case TOKEN_BEGIN_ARRAY: printf("token: [start-array]\n"); return;
        case TOKEN_END_ARRAY: printf("token: [end-array]\n"); return;
        case TOKEN_NAME_SEPARATOR: printf("token: [name separator]\n"); return;
        case TOKEN_VALUE_SEPARATOR: printf("token: [value separator]\n"); return;
        case TOKEN_EOF: printf("token: [eof]\n"); return;

        case TOKEN_STRING:
        case TOKEN_NUMBER:
        case TOKEN_TRUE:
        case TOKEN_FALSE:
        case TOKEN_NULL:
        case TOKEN_ERROR:
            print_full_token(token);
            return;
    }
}

