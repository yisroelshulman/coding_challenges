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
    TOKEN_MINUS,
    TOKEN_QUOTATION,

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
    const TokenType type;
    const char* start;
    const unsigned int len;
    const unsigned int line;
} Token;

#endif