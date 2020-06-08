#ifndef TOKEN_H
#define TOKEN_H

#include <text.h>

enum token_type
{
    ILLEGAL,
    END,
    IDENT,
    INT,
    STRING,
    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    /* 10 */
    SLASH,
    LT,
    GT,
    EQ,
    NOT_EQ,    
    COMMA,
    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    /* 20 */
    RBRACE,
    LBRAKET,
    RBRAKET,
    COLON,
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELZE,
    /* 30 */
    RET
};

struct token
{
    enum token_type type;
    Text_T literal;
} token;

const char *token_type_name(enum token_type type);

#endif
