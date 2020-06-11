#ifndef AST_H
#define AST_H

#include <seq.h>
#include "token.h"

enum statement_type
{
    LET_STMT,
    RETURN_STMT,
    EXPR_STMT
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
    Seq_T statements;
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

struct return_statement
{
    enum statement_type type;
    struct token token;
    struct expression *return_value;
};

struct expression_statement
{
    enum statement_type type;
    struct token token;
    struct expression *expression;
};

Text_T expression_token_literal(struct expression *expression);
Text_T statement_token_literal(struct statement *statement);
Text_T program_token_literal(struct program *program);
Text_T identifier_token_literal(struct identifier *identifier);
Text_T let_statement_token_literal(struct let_statement *let_statement);
Text_T return_statement_token_literal(struct return_statement *let_statement);
Text_T expression_statement_token_literal(struct expression_statement *expression_statement);

char *expression_to_string(struct expression *expression);
char * statement_to_string(struct statement *statement);
const char * program_to_string(struct program *program);
char * identifier_to_string(struct identifier *identifier);
char * let_statement_to_string(struct let_statement *let_statement);
char * return_statement_to_string(struct return_statement *let_statement);
char * expression_statement_to_string(struct expression_statement *expression_statement);

struct program *program_alloc(void);
struct identifier *identifier_alloc(struct token token, Text_T value);
struct return_statement *return_statement_alloc(struct token token);
struct let_statement *let_statement_alloc(struct token token);
struct expression_statement *expression_statement_alloc(struct token token);


void expression_destroy(struct expression *expression);
void statement_destroy(struct statement *statement);
void program_destroy(struct program *program);
void identifier_destroy(struct identifier *identifier);
void let_statement_destroy(struct let_statement *let_statement);
void return_statement_destroy(struct return_statement *return_statement);
void expression_statement_destroy(struct expression_statement *expression_statement);

void program_append_statement(struct program *program,
                              struct statement *statement);




#endif


