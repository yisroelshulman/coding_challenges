#include <stdbool.h>
#include <stdio.h>

#include "json_scanner.h"

typedef struct
{
    Token previous;
    Token current;
    bool had_error;
    bool panic_mode;
} Parser;

Parser parser;


static void parse_value();

static void report_error_token()
{
    parser.had_error = true;

}

static void report_parse_error(const char *message)
{
    parser.had_error = true;
    if (parser.panic_mode) { return; }
}

static void advance()
{
    parser.previous = parser.current;
    parser.current = scan_token();
    if (parser.current.type == TOKEN_ERROR)
    {
        report_error_token();
    }
}

static const bool match(TokenType type)
{
    if (parser.current.type == type)
    {
        advance();
        return true;
    }
    return false;
}

/* member = string: value */
static void parse_member()
{
    if (!match(TOKEN_STRING))
    {
        report_parse_error("Tried to parse a string and didnt find one.");
    }
    if (!match(TOKEN_NAME_SEPARATOR))
    {
        report_parse_error("Tried to parse a name separator but didnt find one.");
    }
    parse_value();
    return;
}

/* object = { member *(, member) } */
static void parse_object()
{
    advance();
    if (match(TOKEN_END_OBJECT)) { return; }
    parse_member();
    while (match(TOKEN_VALUE_SEPARATOR))
    {
        parse_member();
    }
    if (!match(TOKEN_END_OBJECT))
    {
        report_parse_error("Expected '}' to end an object or ',' to to seperate members.");
    }
    return;
}

/*array = [ value *(value) ]*/
static void parse_array()
{
    advance();
    if (match(TOKEN_END_ARRAY)) { return; }
    parse_value();
    while (match(TOKEN_VALUE_SEPARATOR))
    {
        parse_value();
    }
    if (!match(TOKEN_END_ARRAY))
    {
        report_parse_error("Expected ']' to end an array or ',' to separate values.");
    }
    return;
}

/*value = object | array | string | number | true | false | null*/
static void parse_value()
{
    switch (parser.current.type)
    {
        case TOKEN_BEGIN_OBJECT: parse_object(); return;
        case TOKEN_BEGIN_ARRAY: parse_array(); return;
        case TOKEN_STRING: advance(); return;
        case TOKEN_NUMBER: advance(); return;
        case TOKEN_TRUE: advance(); return;
        case TOKEN_FALSE: advance(); return;
        case TOKEN_NULL: advance(); return;
        default:
            report_parse_error("Tried to parse a value but non was found.");
            return;
    }
}

static void start_parse()
{
    switch (parser.current.type)
    {
        case TOKEN_BEGIN_OBJECT: parse_object(); return;
        case TOKEN_BEGIN_ARRAY: parse_array(); return;
        default:
            report_parse_error("Tried to parse a JSON file but errd.");
            return;
    }
}

const bool parse(const char *source)
{
    parser.had_error = false;
    parser.panic_mode = false;
    init_scanner(source);
    parser.current = scan_token();
    parser.previous = parser.current;

    start_parse();

    if (!match(TOKEN_EOF))
    {
        report_parse_error("Found extra data in the file.");
    }

    return !parser.had_error;
}