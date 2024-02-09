#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

#include "json_scanner.h"

/********************************************************************************
 * The Scanner keeps track of which character in the JSON document the scanner
 * is currently on and on which line.
 ********************************************************************************/
typedef struct
{
    const char *start;
    const char *current;
    unsigned int line;
} Scanner;

/*global scanner for the JSON documnet*/
Scanner scanner;

/*initializes the scanner to the start of the JSON document and the first line*/
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

/*advances the scanner one char and return the previous char*/
static const char advance()
{
    scanner.current++;
    return scanner.current[-1];
}

/*digits = [0 - 9]*/
static const bool is_digit(const char c)
{
    return c >= '0' && c <= '9';
}

/* hex digits = [0 - F] case insensitive*/
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

/*whitespaces = newline, tab, space, and return*/
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

/********************************************************************************
 * creates a token with the information from the scanner. If the token is an
 * error token the error message is sent as a parameter. This is so the scanner
 * can also highlight the code snippet.
 ********************************************************************************/
static Token make_token(const TokenType type, const char *message)
{
    Token token = {
        .type = type,
        .start = scanner.start,
        .len = scanner.current - scanner.start,
        .message = message,
        .msg_len = strlen(message),
        .line = scanner.line
    };
    return token;
}

/*fraction = .digit *(digit)*/
static const bool fraction()
{
    advance();
    if (!is_digit(peek())) { return false; }
    while (is_digit(peek())) { advance(); }
    return true;
}

/*exponent = e|E (+|-) digit *(digit)*/
static const bool exponent()
{
    advance();
    if (peek() == '-' || peek() == '+') { advance(); }
    if (!is_digit(peek())) { return false; }
    while (is_digit(peek())) { advance(); }
    return true;
}

/********************************************************************************
 * number = 0|[1 - 9] *(digit) with optional fraction and/or optional exponent
 * fraction = .digit *(digit)
 * exponent =  e|E (+|-) difit *(digit)
 ********************************************************************************/
static Token number(const char c)
{
    if (c == '0' && is_digit(peek())) { return make_token(TOKEN_ERROR, "Whole part of number can't have leading zeroes."); }
    while (is_digit(peek())) { advance(); }
    if (peek() == '.') // has fraction
    {
        if (!fraction()) { return make_token(TOKEN_ERROR, "need at least one digit for fraction."); }
    }
    if (is_exponent(peek())) // has exponent
    {
        if (!exponent()) { return make_token(TOKEN_ERROR, "need at least one digit for fraction."); }
    }
    return make_token(TOKEN_NUMBER, "");
}

/********************************************************************************
 * negative number = - number
 * number = 0|[1 - 9] *(digit) with optional fraction and/or optional exponent
 * fraction = .digit *(digit)
 * exponent =  e|E (+|-) digit *(digit)
 ********************************************************************************/
static Token negative_number()
{
    if (is_digit(peek())) return number(advance());
    return make_token(TOKEN_ERROR, "Negative witout a number.");
}

static const bool match(const int start, const char* rest, const int len)
{
    return ((scanner.current - scanner.start) == (start + len)) && memcmp(scanner.start + start, rest, len) == 0;
}

/*keywords = false, true, and null*/
static Token keyword()
{
    while (is_alpha(peek())) advance(); // advance until a non-letter is reached
    switch (scanner.start[0])
    {
        case 'f':
            if (match(1, "alse", 4)) { return make_token(TOKEN_FALSE, ""); }
            break;
        case 't':
            if (match(1, "rue", 3)) { return make_token(TOKEN_TRUE, ""); }
            break;
        case 'n':
        if (match(1, "ull", 3)) { return make_token(TOKEN_NULL, ""); }
        break;
    }
    return make_token(TOKEN_ERROR, "Invalid keyword.");
}

/********************************************************************************
 * escaped = ", \, /, b, f, n, r, t, and uXXXX where x is a hex digit. This
 * allows unicode characters to be entered by their hex value.
 ********************************************************************************/
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
                if (!is_hex(peek())) { return false; }
            }
            return true;
    }
    return false;
}

/********************************************************************************
 * string = " *(characters|\escaped) "
 * characters = unicode characters >= 0x20 excluding the \ and " characters.
 * escaped = ", \, /, b, f, n, r, t, and uXXXX where x is a hex digit. This
 * allows unicode characters to be entered by their hex value.
 ********************************************************************************/
static Token string()
{
    while (!(peek() == '"') && !(is_at_end())) {
        if (peek() < 32) { return make_token(TOKEN_ERROR, "Not a valid character in string."); }
        if (peek() == '\\') {
            advance();
            if (!escaped()) { return make_token(TOKEN_ERROR, "Invalid escaped character."); }
        }
        advance();
    }
    if (is_at_end()) { return make_token(TOKEN_ERROR, "Reached end of file without a string terminator."); }
    advance(); // move past the closing '"'
    return make_token(TOKEN_STRING, "");
}

/********************************************************************************
 * scans a single token and creates a token struct to return to the caller
 * There are 13 tokens types that can be created.
 * Each token holds the information of where in the document the tokens lexeme
 * is and the length. If there was an error the token also contains a message
 * related to the error.
 ********************************************************************************/
Token scan_token()
{
    consume_whitespaces();
    if (is_at_end()) { return make_token(TOKEN_EOF, ""); }
    scanner.start = scanner.current;
    const char c = advance();

    if (is_digit(c)) { return number(c); }
    if (is_alpha(c)) { return keyword(); }

    switch (c)
    {
        case '{': return make_token(TOKEN_BEGIN_OBJECT, "");
        case '}': return make_token(TOKEN_END_OBJECT, "");
        case '[': return make_token(TOKEN_BEGIN_ARRAY, "");
        case ']': return make_token(TOKEN_END_ARRAY, "");
        case ':': return make_token(TOKEN_NAME_SEPARATOR, "");
        case ',': return make_token(TOKEN_VALUE_SEPARATOR, "");
        case '-': return negative_number(c);
        case '"': return string();
    }
    return make_token(TOKEN_ERROR, "Unrecognized character.");
}