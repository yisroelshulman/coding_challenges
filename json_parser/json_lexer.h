#ifndef JSON_LEXER_H
#define JSON_LEXER_H

typedef enum {
    // symbols
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_QUOTATION,
    TOKEN_ESCAPE,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_DOT,

    // literals
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL
} TokenType;

#endif