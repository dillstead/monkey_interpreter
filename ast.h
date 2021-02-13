#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include <seq.h>
#include "token.h"

enum node_type
{
    PROGRAM,
    LET_STMT,
    RETURN_STMT,
    EXPR_STMT,
    BLOCK_STMT,
    IDENT_EXPR,
    STRING_LITERAL_EXPR,
    INT_LITERAL_EXPR,
    ARRAY_LITERAL_EXPR,
    HASH_LITERAL_EXPR,
    FUNC_LITERAL_EXPR,
    INDEX_EXPR,
    PREFIX_EXPR,
    INFIX_EXPR,
    BOOL_EXPR,
    IF_EXPR,
    CALL_EXPR
};

extern const char *node_type_str[];

struct node
{
    enum node_type type;
};

struct statement
{
    enum node_type type;
    struct token token;
};

struct expression
{
    enum node_type type;
    struct token token;
};

struct program
{
    enum node_type type;
    Seq_T statements;
};

struct identifier
{
    enum node_type type;
    struct token token;
    Text_T value;
};

struct string_literal
{
    enum node_type type;
    struct token token;
    Text_T value;
};

struct integer_literal
{
    enum node_type type;
    struct token token;
    long long value;
};

struct array_literal
{
    enum node_type type;
    struct token token;
    Seq_T elements;
};

struct hash_literal
{
    enum node_type type;
    struct token token;
    Seq_T keys;
    Seq_T values;
};

struct boolean
{
    enum node_type type;
    struct token token;
    bool value;
};

struct function_literal
{
    enum node_type type;
    struct token token;
    unsigned int cnt;
    Seq_T parameters;
    struct block_statement *body;
};

struct let_statement
{
    enum node_type type;
    struct token token;
    struct identifier *name;
    struct expression *value;
};

struct return_statement
{
    enum node_type type;
    struct token token;
    struct expression *return_value;
};

struct expression_statement
{
    enum node_type type;
    struct token token;
    struct expression *expression;
};

struct block_statement
{
    enum node_type type;
    struct token token;
    Seq_T statements;
};

struct index_expression
{
    enum node_type type;
    struct token token;
    struct expression *left;
    struct expression *index;    
};

struct prefix_expression
{
    enum node_type type;
    struct token token;
    Text_T op;
    struct expression *right;
};

struct infix_expression
{
    enum node_type type;
    struct token token;
    struct expression *left;
    Text_T op;
    struct expression *right;
};

struct if_expression
{
    enum node_type type;
    struct token token;
    struct expression *condition;
    struct block_statement *consequence;
    struct block_statement *alternative;
};

struct call_expression
{
    enum node_type type;
    struct token token;
    struct expression *function;
    Seq_T arguments;
};

Text_T expression_token_literal(struct expression *expression);
Text_T statement_token_literal(struct statement *statement);
Text_T program_token_literal(struct program *program);
Text_T identifier_token_literal(struct identifier *identifier);
Text_T string_literal_token_literal(struct string_literal *string_literal);
Text_T integer_literal_token_literal(struct integer_literal *integer_literal);
Text_T array_literal_token_literal(struct array_literal *array_literal);
Text_T hash_literal_token_literal(struct hash_literal *hash_literal);
Text_T function_literal_token_literal(struct function_literal *function_literal);
Text_T boolean_token_literal(struct boolean *boolean);
Text_T let_statement_token_literal(struct let_statement *let_statement);
Text_T return_statement_token_literal(struct return_statement *let_statement);
Text_T expression_statement_token_literal(struct expression_statement *expression_statement);
Text_T block_statement_token_literal(struct block_statement *block_statement);
Text_T prefix_expression_token_literal(struct prefix_expression *prefix_expression);
Text_T index_expression_token_literal(struct index_expression *index_expression);
Text_T infix_expression_token_literal(struct infix_expression *infix_expression);
Text_T if_expression_token_literal(struct if_expression *if_expression);
Text_T call_expression_token_literal(struct call_expression *call_expression);
char *expression_to_string(struct expression *expression);
char *statement_to_string(struct statement *statement);
char *program_to_string(struct program *program);
char *identifier_to_string(struct identifier *identifier);
char *string_literal_to_string(struct string_literal *string_literal);
char *integer_literal_to_string(struct integer_literal *integer_literal);
char *array_literal_to_string(struct array_literal *array_literal);
char *hash_literal_to_string(struct hash_literal *hash_literal);
char *function_literal_to_string(struct function_literal *function_literal);
char *boolean_to_string(struct boolean *boolean);
char *let_statement_to_string(struct let_statement *let_statement);
char *return_statement_to_string(struct return_statement *let_statement);
char *expression_statement_to_string(struct expression_statement *expression_statement);
char *block_statement_to_string(struct block_statement *block_statement);
char *index_expression_to_string(struct index_expression *index_expression);
char *prefix_expression_to_string(struct prefix_expression *prefix_expression);
char *infix_expression_to_string(struct infix_expression *infix_expression);
char *if_expression_to_string(struct if_expression *if_expression);
char *call_expression_to_string(struct call_expression *call_expression);
struct program *program_alloc(void);
struct identifier *identifier_alloc(struct token token);
struct string_literal *string_literal_alloc(struct token token);
struct integer_literal *integer_literal_alloc(struct token token);
struct array_literal *array_literal_alloc(struct token token);
struct hash_literal *hash_literal_alloc(struct token token);
struct function_literal *function_literal_alloc(struct token token);
void function_literal_addref(struct function_literal *function_literal);
struct boolean *boolean_alloc(struct token token, bool value);
struct return_statement *return_statement_alloc(struct token token);
struct let_statement *let_statement_alloc(struct token token);
struct expression_statement *expression_statement_alloc(struct token token);
struct block_statement *block_statement_alloc(struct token token);
struct index_expression *index_expression_alloc(struct token token);
struct prefix_expression *prefix_expression_alloc(struct token token);
struct infix_expression *infix_expression_alloc(struct token token);
struct if_expression *if_expression_alloc(struct token token);
struct call_expression *call_expression_alloc(struct token token);
void expression_destroy(struct expression *expression);
void statement_destroy(struct statement *statement);
void program_destroy(struct program *program);
void identifier_destroy(struct identifier *identifier);
void string_literal_destroy(struct string_literal *string_literal);
void integer_literal_destroy(struct integer_literal *integer_literal);
void array_literal_destroy(struct array_literal *array_literal);
void hash_literal_destroy(struct hash_literal *hash_literal);
void function_literal_destroy(struct function_literal *function_literal);
void boolean_destroy(struct boolean *boolean);
void let_statement_destroy(struct let_statement *let_statement);
void return_statement_destroy(struct return_statement *return_statement);
void expression_statement_destroy(struct expression_statement *expression_statement);
void block_statement_destroy(struct block_statement *block_statement);
void index_expression_destroy(struct index_expression *index_expression);
void prefix_expression_destroy(struct prefix_expression *prefix_expression);
void infix_expression_destroy(struct infix_expression *infix_expression);
void if_expression_destroy(struct if_expression *if_expression);
void call_expression_destroy(struct call_expression *call_expression);
void program_append_statement(struct program *program,
                              struct statement *statement);
void block_statement_append_statement(struct block_statement *block_statement,
                                      struct statement *statement);
#endif


