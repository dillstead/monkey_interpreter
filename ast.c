#include <mem.h>

#include "ast.h"

static Text_T empty = { 0, "" };

Text_T program_token_literal(struct program *program)
{
    if (Seq_length(program->statements) > 0)
    {
        struct statement *statement
            = (struct statement *) Seq_get(program->statements, 0);
        return statement_token_literal(statement);
    }
    else
    {
        return empty;
    }
}

Text_T expression_token_literal(struct expression *expression)
{
    
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        return identifier_token_literal((struct identifier *) expression);
    }
    default:
    {
        return empty;
    }
    }
}

Text_T statement_token_literal(struct statement *statement)
{
    switch (statement->type)
    {
    case LET_STMT:
    {
        return let_statement_token_literal((struct let_statement *) statement);
    }
    case RETURN_STMT:
    {
        return return_statement_token_literal((struct return_statement *) statement);
    }
    default:
    {
        return empty;
    }
    }
}

Text_T let_statement_token_literal(struct let_statement *let_statement)
{
    return let_statement->token.literal;
}

Text_T return_statement_token_literal(struct return_statement *return_statement)
{
    return return_statement->token.literal;
}

Text_T identifier_token_literal(struct identifier *identifier)
{
    return identifier->token.literal;
}

void expression_destroy(struct expression *expression)
{
    if (expression == NULL)
    {
        return;
    }
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        identifier_destroy((struct identifier *) expression);
        break;
    }
    default:
    {
    }
    }
}

void statement_destroy(struct statement *statement)
{
    if (statement == NULL)
    {
        return;
    }
    switch (statement->type)
    {
    case LET_STMT:
    {
        let_statement_destroy((struct let_statement *) statement);
        break;
    }
    case RETURN_STMT:
    {
        return_statement_destroy((struct return_statement *) statement);
        break;
    }
    default:
    {
    }
    }
}

struct program *program_alloc(void)
{
    struct program *program;

    NEW0(program);
    program->statements = Seq_new(100);
    return program;
}

void program_destroy(struct program *program)
{
    struct statement *statement;
    
    for (int i = 0; i < Seq_length(program->statements); i++)
    {
        statement = (struct statement *) Seq_get(program->statements, i);
        statement_destroy(statement);
    }
    Seq_free(&program->statements);
    FREE(program);
}

void program_append_statement(struct program *program,
                              struct statement *statement)
{
    Seq_addhi(program->statements, statement);
}

struct let_statement *let_statement_alloc(struct token token)
{
    struct let_statement *let_statement;

    NEW0(let_statement);
    let_statement->type = LET_STMT;
    let_statement->token = token;
    return let_statement;
}

struct return_statement *return_statement_alloc(struct token token)
{
    struct return_statement *return_statement;

    NEW0(return_statement);
    return_statement->type = RETURN_STMT;
    return_statement->token = token;
    return return_statement;
}

void let_statement_destroy(struct let_statement *let_statement)
{
    identifier_destroy(let_statement->name);
    expression_destroy(let_statement->value);
    FREE(let_statement);
}

void return_statement_destroy(struct return_statement *return_statement)
{
    expression_destroy(return_statement->return_value);
    FREE(return_statement);
}

struct identifier *identifier_alloc(struct token token, Text_T value)
{
    struct identifier *identifier;

    NEW0(identifier);
    identifier->type = IDENT_EXPR;
    identifier->token = token;
    identifier->value = value;
    return identifier;
}

void identifier_destroy(struct identifier *identifier)
{
    FREE(identifier);
}
