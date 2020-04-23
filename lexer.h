#ifndef LEXER_H
#define LEXER_H

#include <text.h>
#include "token.h"

struct lexer
{
    int input_length;
    const char *input;
    int position;
    int read_position;
    char ch;
};

void setup_lexer(void);
void lexer_init(struct lexer *lexer, const char *input);
struct token lexer_next_token(struct lexer *lexer);

#endif
