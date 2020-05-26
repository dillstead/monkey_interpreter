#include <mem.h>

#include "parser.h"

static void next_token(struct parser *parser)
{
    parser->cur_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

void parser_init(void)
{
    
}

struct parser *parser_alloc(struct lexer *lexer)
{
    struct parser *parser;

    NEW0(parser);
    parser->lexer = lexer;
    next_token(parser);
    next_token(parser);
    return parser;
}

void parser_destroy(struct parser *parser)
{
    FREE(parser);
}

struct program *parser_parse_program(struct parser *parser)
{
    return NULL;
}
