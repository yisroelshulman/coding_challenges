#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "json_scanner.h"

typedef struct
{
    const char *start;
    const char *current;
    unsigned int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source)
{
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

static const char peek()
{
    return *scanner.current;
}

static const bool is_at_end()
{
    return *scanner.current == '\0';
}

static const char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

static const bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

static const bool is_hex(const char c)
{
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static const bool is_exponent(const char c)
{
    return c == 'e' || c == 'E';
}

static const bool is_alpha(const char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static void consume_whitespaces()
{
    for (;;)
    {
        switch (peek())
        {
            case '\n': scanner.line++;
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            default:
                return;
        }
    }
}

static Token make_token(const TokenType type)
{
    Token token = {
        .type = type,
        .start = scanner.start,
        .len = scanner.current - scanner.start,
        .line = scanner.line
    };
    return token;
}

static Token error_token(const char *message)
{
    Token token = {
        .type = TOKEN_ERROR,
        .start = message,
        .len = strlen(message),
        .line = scanner.line,
    };
    return token;
}

static Token number(const char c)
{
    if (c == '0' && is_digit(peek())) return error_token("Whole part of number can't have leading zeroes.");
    while (is_digit(peek())) advance();
    if (peek() == '.')
    {
        advance();
        if (!is_digit(peek())) return error_token("need at least one digit for fraction.");
        while (is_digit(peek())) advance();
    }
    if (is_exponent(peek()))
    {
        advance();
        if (peek() == '-' || peek() == '+') advance();
        if (!is_digit(peek())) return error_token("need at least one digit for exponent.");
        while (is_digit(peek())) advance();
    }
    return make_token(TOKEN_NUMBER);
}

static Token negative_number(const char c)
{
    if (is_digit(c)) return number(c);
    return error_token("Not a valid digit.");
}

static const bool match(const int start, const char* rest, const int len)
{
    return ((scanner.current - scanner.start) == (start + len)) && memcmp(scanner.start + start, rest, len);
}

static Token keyword()
{
    while (is_alpha(peek())) advance();
    switch (scanner.start[0])
    {
        case 'f': if (match(1, "alse", 4)) return make_token(TOKEN_FALSE);
        case 't': if (match(1, "rue", 3)) return make_token(TOKEN_TRUE);
        case 'n': if (match(1, "ull", 3)) return make_token(TOKEN_NULL);
    }
    return error_token("Invalid keyword.");
}

static const bool escaped()
{
    switch (peek())
    {
        case '\\':
        case '"':
        case '/':
        case 'b':
        case 'f':
        case 'n':
        case 'r':
        case 't':
            return true;
        case 'u':
            for (int i = 0; i < 4; i++)
            {
                advance();
                if (!is_hex(peek())) return false;
            }
            return true;
    }
    return false;
}

static Token string()
{
    while (!(peek() == '"') && !(is_at_end())) {
        if (peek() == '\\') {
            advance();
            if (!escaped()) return error_token("Invalid escaped character.");
        }
        advance();
    }
    if (is_at_end()) return error_token("Reached end of file withought a string terminator.");
    advance(); // move past the closing '"'
    return make_token(TOKEN_STRING);
}

Token scan_token()
{
    consume_whitespaces();
    if (is_at_end()) return make_token(TOKEN_EOF);
    scanner.start = scanner.current;
    const char c = advance();

    if (is_digit(c)) return number(c);
    if (is_alpha(c)) return keyword();

    switch (c)
    {
        case '{': return make_token(TOKEN_BEGIN_OBJECT);
        case '}': return make_token(TOKEN_END_OBJECT);
        case '[': return make_token(TOKEN_BEGIN_ARRAY);
        case ']': return make_token(TOKEN_END_ARRAY);
        case ':': return make_token(TOKEN_NAME_SEPARATOR);
        case ',': return make_token(TOKEN_VALUE_SEPARATOR);
        case '-': return negative_number(c);
        case '"': return string();
    }
    return error_token("Unrecognized character.");
}