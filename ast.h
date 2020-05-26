#ifndef AST_H
#define AST_H

#include <array.h>
#include "token.h"

enum statement_type
{
    LET_STMT
};

struct statement
{
    enum statement_type type;
    struct token token;
};


enum expression_type
{
    IDENT_EXPR
};

struct expression
{
    enum expression_type type;
    struct token token;
};

struct program
{
    Array_T statements;
};

struct identifier
{
    enum expression_type type;
    struct token token;
    Text_T value;
};

struct let_statement
{
    enum statement_type type;
    struct token token;
    struct identifier *name;
    struct expression *value;
};

Text_T expression_token_literal(struct expression *expression);
Text_T statement_token_literal(struct statement *statement);
Text_T program_token_literal(struct program *program);
Text_T let_statement_token_literal(struct let_statement *let_statement);
Text_T identifier_token_literal(struct identifier *identifier);
void expression_destroy(struct expression *expression);
void statement_destroy(struct statement *statement);
void program_destroy(struct program *program);
void let_statement_destroy(struct let_statement *let_statement);
void identifier_destroy(struct identifier *identifier);
#endif


