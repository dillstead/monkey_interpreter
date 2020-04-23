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
    RET
};
  
struct token
{
    enum token_type type;
    Text_T literal;
} token;

#endif
