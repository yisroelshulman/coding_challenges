#ifndef JSON_SCANNER_H
#define JSON_SCANNER_H

typedef enum
{
    // symbols
    TOKEN_BEGIN_OBJECT,
    TOKEN_END_OBJECT,
    TOKEN_BEGIN_ARRAY,
    TOKEN_END_ARRAY,
    TOKEN_NAME_SEPARATOR,
    TOKEN_VALUE_SEPARATOR,

    // literals
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,

    // other
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct
{
    TokenType type;
    const char* start;
    unsigned int len;
    unsigned int line;
} Token;

void init_scanner(const char *source);
Token scan_token();

#endif