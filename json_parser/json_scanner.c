#include <stdbool.h>
#include <stdlib.h>

#include "json_scanner.h"

typedef struct {
    const char *start;
    const char *current;
    unsigned int line;
} Scanner;

Scanner scanner;

void init_scanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.line = 1;
}

char peek() {
    return *scanner.current;
}

bool is_at_end() {
    return *scanner.current == '\0';
}

char advance() {
    scanner.current++;
    return scanner.current[-1];
}

void consume_whitespaces() {
    for (;;) {
        switch (peek()) {
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

Token make_token(const TokenType type) {
    Token token = {
        .type = type,
        .start = scanner.start,
        .len = scanner.current - scanner.start,
        .line = scanner.line
    };
    return token;
}

Token scan_token() {

    consume_whitespaces();
    if (is_at_end()) return make_token(TOKEN_EOF);

    scanner.start = scanner.current;

    const char c = advance();

    switch (c) {
        case '{': return make_token(TOKEN_BEGIN_OBJECT);
        case '}': return make_token(TOKEN_END_OBJECT);
        case '[': return make_token(TOKEN_BEGIN_ARRAY);
        case ']': return make_token(TOKEN_END_ARRAY);
        case ':': return make_token(TOKEN_NAME_SEPARATOR);
        case ',': return make_token(TOKEN_VALUE_SEPARATOR);
    }
    return make_token(TOKEN_ERROR);
}