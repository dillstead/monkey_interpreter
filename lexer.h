#ifndef LEXER_H
#define LEXER_H

struct lexer
{
    int input_length;
    const char *input;
    int position;
    int read_position;
    char ch;
};

void lexer_init(void);
struct lexer *lexer_alloc(const char *input);
void lexer_destroy(struct lexer *lexer);
struct token lexer_next_token(struct lexer *lexer);
#endif
