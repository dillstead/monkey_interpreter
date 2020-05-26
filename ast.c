#include <mem.h>

#include "ast.h"

static Text_T empty = { 0, "" };

Text_T program_token_literal(struct program *program)
{
    if (Array_length(program->statements) > 0)
    {
        struct statement *statement
            = (struct statement *) Array_get(program->statements, 0);
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

Text_T identifier_token_literal(struct identifier *identifier)
{
    return identifier->token.literal;
}

void expression_destroy(struct expression *expression)
{
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        identifier_destroy((struct identifier *) expression);
    }
    default:
    {
    }
    }
}

void statement_destroy(struct statement *statement)
{
    switch (statement->type)
    {
    case LET_STMT:
    {
        let_statement_destroy((struct let_statement *) statement);
    }
    default:
    {
    }
    }
}

void program_destroy(struct program *program)
{
    struct statement *statement;
    
    for (int i = 0; i < Array_size(program->statements); i++)
    {
        statement = (struct statement *) Array_get(program->statements, i);
        statement_destroy(statement);
    }
    Array_free(&program->statements);
    FREE(program);
}

void let_statement_destroy(struct let_statement *let_statement)
{
    identifier_destroy(let_statement->name);
    expression_destroy(let_statement->value);
    FREE(let_statement);
}

void identifier_destroy(struct identifier *identifier)
{
    FREE(identifier);
}
