#ifndef PARSER_H
#define PARSER_H

#include <seq.h>
#include "lexer.h"
#include "token.h"
#include "ast.h"

struct parser
{
    struct lexer *lexer;
    struct token cur_token;
    struct token peek_token;
    Seq_T errors;
};

void parser_init(void);
struct parser *parser_alloc(struct lexer *lexer);
void parser_destroy(struct parser *parser);
struct program *parser_parse_program(struct parser *parser);
Seq_T parser_errors(struct parser *parser);
#endif
