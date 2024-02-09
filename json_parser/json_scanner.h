#ifndef JSON_SCANNER_H
#define JSON_SCANNER_H

/*All the tokens that can be created when parsing a JSON file*/
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

/*The token information*/
typedef struct
{
    TokenType type;
    const char *start;
    unsigned int len;
    const char *message;
    unsigned int msg_len;
    unsigned int line;
} Token;

/*function prototypes for interfacing with the json_scanner*/

/*initializes the scanner to the start of the JSON document and the first line*/
void init_scanner(const char *source);

/********************************************************************************
 * scans a single token and creates a token struct to return to the caller
 * There are 13 tokens types that can be created.
 * Each token holds the information of where in the document the tokens lexeme
 * is and the length. If there was an error the token also contains a message
 * related to the error.
 ********************************************************************************/
Token scan_token();

#endif