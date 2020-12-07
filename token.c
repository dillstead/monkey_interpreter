#include <mem.h>

#include "token.h"

static const char *name[] =
{
    "ILLEGAL",
    "END",
    "IDENT",
    "INT",
    "STRING",
    "=",
    "+",
    "-",
    "!",
    "*",
    "/",
    "<",
    ">",
    "==",
    "!=",    
    ",",
    ";",
    "(",
    ")",
    "[",
    "]",
    "{",
    "}",
    ":",
    "FUNCTION",
    "LET",
    "TRUE",
    "FALSE",
    "IF",
    "ELZE",
    "RET"
};

const char *token_type_name(enum token_type type)
{
    return name[type];
}
