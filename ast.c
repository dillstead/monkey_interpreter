#include <mem.h>
#include <str.h>

#include "ast.h"

static Text_T empty = { 0, "" };

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

Text_T identifier_token_literal(struct identifier *identifier)
{
    return identifier->token.literal;
}

Text_T let_statement_token_literal(struct let_statement *let_statement)
{
    return let_statement->token.literal;
}

Text_T return_statement_token_literal(struct return_statement *return_statement)
{
    return return_statement->token.literal;
}

Text_T expression_statement_token_literal(struct expression_statement *expression_statement)
{
    return expression_statement->token.literal;
}

char *expression_to_string(struct expression *expression)
{
    
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        return identifier_to_string((struct identifier *) expression);
    }
    default:
    {
        return NULL;
    }
    }
}

char *statement_to_string(struct statement *statement)
{
    switch (statement->type)
    {
    case LET_STMT:
    {
        return let_statement_to_string((struct let_statement *) statement);
    }
    case RETURN_STMT:
    {
        return return_statement_to_string((struct return_statement *) statement);
    }
    case EXPR_STMT:
    {
        return expression_statement_to_string((struct expression_statement *) statement);
    }
    default:
    {
        return NULL;
    }
    }
}

const char *program_to_string(struct program *program)
{
    struct statement *statement;
    char *str;
    char *str1;
    char *str2;
        
    if (Seq_length(program->statements) > 0)
    {
        statement = (struct statement *) Seq_get(program->statements, 0);
        str = statement_to_string(statement);
        str1 = str;
        for (int i = 1; i < Seq_length(program->statements); i++)
        {
            statement = (struct statement *) Seq_get(program->statements, i);
            str2 = statement_to_string(statement);
            str = Str_cat(str1, 1, 0, str2, 1, 0);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
        return str;
    }
    else
    {
        return NULL;
    }
}

char *identifier_to_string(struct identifier *identifier)
{
    int len;
    char *str;

    len = identifier->token.literal.len + 1;
    str = ALLOC(len);
    Fmt_sfmt(str, len, "%T", identifier->token.literal);
    return str;
}

char *let_statement_to_string(struct let_statement *let_statement)
{
    int len;
    char *str;
    char *str1;
    char *str2;
    
    len = let_statement->token.literal.len + 2;
    str = ALLOC(len);
    str1 = str;
    Fmt_sfmt(str, len, "%T ", &let_statement->token.literal);
    str2 = identifier_to_string(let_statement->name);
    str = Str_catv(str1, 0, 1, str2, 0, 1, " = ", 0, 1);
    FREE(str2);
    FREE(str1);
    str1 = str;
    if (let_statement->value != NULL)
    {
        str2 = expression_to_string(let_statement->value);
        str = Str_cat(str1, 0, 1, str2, 0, 1);
        FREE(str2);
        FREE(str1);
        str1 = str;
    }
    str = Str_cat(str1, 0, 1, ";", 0, 1);
    FREE(str1);
    return str;
}

char *return_statement_to_string(struct return_statement *return_statement)
{
    int len;
    char *str;
    char *str1;
    char *str2;

    len = return_statement->token.literal.len + 2;
    str = ALLOC(len);
    str1 = str;
    Fmt_sfmt(str, len, "%T ", &return_statement->token.literal);
    if (return_statement->return_value != NULL)
    {
        str2 = expression_to_string(return_statement->return_value);
        str = Str_cat(str1, 0, 1, str2, 0, 1);
        FREE(str2);
        FREE(str1);
        str1 = str;
    }
    str = Str_cat(str1, 0, 1, ";", 0, 1);
    FREE(str1);
    return str;
}

char *expression_statement_to_string(struct expression_statement *expression_statement)
{
    if (expression_statement->expression != NULL)
    {
        return expression_to_string(expression_statement->expression);
    }
    return NULL;
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
