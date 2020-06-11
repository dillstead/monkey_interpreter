#include <stdbool.h>
#include <fmt.h>
#include <mem.h>

#include "parser.h"

#define ERROR_MSG_SZ 128

static void peek_error(struct parser *parser, enum token_type type)
{
    char *msg;

    msg = ALLOC(ERROR_MSG_SZ);
    Fmt_sfmt(msg, ERROR_MSG_SZ, "expected next token to be %s, got %s instead",
             token_type_name(type), token_type_name(parser->peek_token.type));
    Seq_addhi(parser->errors, msg);
}

static void next_token(struct parser *parser)
{
    parser->cur_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

static bool cur_token_is(struct parser *parser, enum token_type type)
{
    return parser->cur_token.type == type;
}

static bool peek_token_is(struct parser *parser, enum token_type type)
{
    return parser->peek_token.type == type;
}

static bool expect_peek(struct parser *parser, enum token_type type)
{
    if (peek_token_is(parser, type))
    {
        next_token(parser);
        return true;
    }
    else
    {
        peek_error(parser, type);
        return false;
    }
}

static struct let_statement *parse_let_statement(struct parser *parser)
{
    struct let_statement *let_statement;

    let_statement = let_statement_alloc(parser->cur_token);
    if (!expect_peek(parser, IDENT))
    {
        return NULL;
    }
    let_statement->name = identifier_alloc(parser->cur_token,
                                          parser->cur_token.literal);
    if (!expect_peek(parser, ASSIGN))
    {
        return NULL;
    }
    while (!cur_token_is(parser, SEMICOLON))
    {
        next_token(parser);
    }
    return let_statement;
}

static struct return_statement *parse_return_statement(struct parser *parser)
{
    struct return_statement *return_statement;

    return_statement = return_statement_alloc(parser->cur_token);
    next_token(parser);
    while (!cur_token_is(parser, SEMICOLON))
    {
        next_token(parser);
    }
    return return_statement;
}
    
static struct statement *parse_statement(struct parser *parser)
{
    switch (parser->cur_token.type)
    {
    case LET:
    {
        return (struct statement *) parse_let_statement(parser);
    }
    case RET:
    {
        return (struct statement *) parse_return_statement(parser);
    }
    default:
    {
        return NULL;
    }
    }
}

void parser_init(void)
{
    Fmt_register('T', Text_fmt);
}

struct parser *parser_alloc(struct lexer *lexer)
{
    struct parser *parser;

    NEW0(parser);
    parser->lexer = lexer;
    next_token(parser);
    next_token(parser);
    parser->errors = Seq_new(100);
    return parser;
}

void parser_destroy(struct parser *parser)
{
    char *msg;
    
    for (int i = 0; i < Seq_length(parser->errors); i++)
    {
        msg = (char *) Seq_get(parser->errors, i);
        FREE(msg);
    }
    Seq_free(&parser->errors);
    FREE(parser);
}

struct program *parser_parse_program(struct parser *parser)
{
    struct program *program = program_alloc();
    struct statement *statement;

    while (parser->cur_token.type != END)
    {
        statement = parse_statement(parser);
        if (statement != NULL)
        {
            program_append_statement(program, statement);
        }
        next_token(parser);
    }
    return program;
}

Seq_T parser_errors(struct parser *parser)
{
    return parser->errors;
}
