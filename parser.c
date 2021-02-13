#include <stdlib.h>
#include <stdbool.h>
#include <fmt.h>
#include <mem.h>
#include <table.h>

#include "parser.h"

#define ERROR_MSG_SZ 128

enum precedence_type
{
    LOWEST_PREC,
    EQUALS_PREC,
    LESSGREATER_PREC,
    SUM_PREC,
    PRODUCT_PREC,
    PREFIX_PREC,
    CALL_PREC,
    INDEX_PREC
};

struct prefix_parse_fn
{
    struct expression *(*fn)(struct parser *);
};

struct infix_parse_fn
{
    struct expression *(*fn)(struct parser *, struct expression *);
};

static Table_T prefix_parse_fns;
static Table_T infix_parse_fns;
static Table_T precedences;

static int int_cmp (const void *x, const void *y)
{
    int ix = *((int *) x);
    int iy = *((int *) y);
    
    if (ix < iy)
    {
        return -1;
    }
    else if (ix > iy)
    {
        return +1;
    }
    else
    {
        return 0;
    }
}

static unsigned int_hash(const void *x)
{
    return *((int *)x);
}

static void peek_error(struct parser *parser, enum token_type type)
{
    char *msg;

    msg = ALLOC(ERROR_MSG_SZ);
    Fmt_sfmt(msg, ERROR_MSG_SZ, "expected next token to be %s, got %s instead",
             token_type_name(type), token_type_name(parser->peek_token.type));
    Seq_addhi(parser->errors, msg);
}

static void no_prefix_parse_fn_error(struct parser *parser, enum token_type type)
{
    char *msg;

    msg = ALLOC(ERROR_MSG_SZ);
    Fmt_sfmt(msg, ERROR_MSG_SZ, "no prefix parse function for %s found",
             token_type_name(type));
    Seq_addhi(parser->errors, msg);
}

static enum precedence_type peek_precedence(struct parser *parser)
{
    return (enum precedence_type) Table_get(precedences,
                                            &parser->peek_token.type);
}

static enum precedence_type cur_precedence(struct parser *parser)
{
    return (enum precedence_type) Table_get(precedences,
                                            &parser->cur_token.type);
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

static struct identifier *parse_identifier(struct parser *parser)
{
    struct identifier *identifier;
    
    identifier = identifier_alloc(parser->cur_token);
    identifier->value = Text_box(Text_get(NULL, 0, parser->cur_token.literal),
                                 parser->cur_token.literal.len);
    return identifier;
}

static struct string_literal *parse_string_literal(struct parser *parser)
{
    struct string_literal *string_literal;
    
    string_literal = string_literal_alloc(parser->cur_token);
    string_literal->value = Text_box(Text_get(NULL, 0, parser->cur_token.literal),
                                     parser->cur_token.literal.len);
    return string_literal;
}

static struct integer_literal *parse_integer_literal(struct parser *parser)
{
    struct integer_literal *integer_literal;
    long long value;
    char *value_str = NULL;
    char *end;
    char *msg;
    bool success = false;

    integer_literal = integer_literal_alloc(parser->cur_token);
    value_str = Text_get(NULL, 0, parser->cur_token.literal);
    value = strtoull(value_str, &end, 10);
    if (*end)
    {
        msg = ALLOC(ERROR_MSG_SZ);
        Fmt_sfmt(msg, ERROR_MSG_SZ, "could not parse %T as integer",
                 &parser->cur_token.literal);
        Seq_addhi(parser->errors, msg);
        goto cleanup;
    }
    integer_literal->value = value;
    success = true;

cleanup:
    FREE(value_str);
    if (!success)
    {
        integer_literal_destroy(integer_literal);
        integer_literal = NULL;
    }
    return integer_literal;
}

static struct boolean *parse_boolean(struct parser *parser)
{
    Text_T t = { sizeof "true" - 1, "true"};
        
    return boolean_alloc(parser->cur_token,
                         !Text_cmp(parser->cur_token.literal, t));
}

static struct expression *parse_expression(struct parser *parser,
                                           enum precedence_type precedence)
{
    struct prefix_parse_fn *prefix_parse_fn;
    struct infix_parse_fn *infix_parse_fn;
    struct expression *left_expression;

    prefix_parse_fn = (struct prefix_parse_fn *)
        Table_get(prefix_parse_fns, &parser->cur_token.type);
    if (prefix_parse_fn == NULL)
    {
        no_prefix_parse_fn_error(parser, parser->cur_token.type);
        return NULL;
    }
    left_expression = prefix_parse_fn->fn(parser);
    while (!peek_token_is(parser, SEMICOLON)
           && precedence < peek_precedence(parser))
    {
        infix_parse_fn = (struct infix_parse_fn *)
            Table_get(infix_parse_fns, &parser->peek_token.type);
        if (infix_parse_fn == NULL)
        {
            return left_expression;
        }
        next_token(parser);
        left_expression = infix_parse_fn->fn(parser, left_expression);
    }
    return left_expression;
}

static struct let_statement *parse_let_statement(struct parser *parser)
{
    struct let_statement *let_statement;
    bool success = false;

    let_statement = let_statement_alloc(parser->cur_token);
    if (!expect_peek(parser, IDENT))
    {
        goto cleanup;
    }
    let_statement->name = parse_identifier(parser);
    if (!expect_peek(parser, ASSIGN))
    {
        goto cleanup;
    }
    next_token(parser);
    let_statement->value = parse_expression(parser, LOWEST_PREC);
    if (peek_token_is(parser, SEMICOLON))
    {
        next_token(parser);
    }
    success = true;

cleanup:
    if (!success)
    {
        let_statement_destroy(let_statement);
        let_statement = NULL;
    }    
    return let_statement;
}

static struct return_statement *parse_return_statement(struct parser *parser)
{
    struct return_statement *return_statement;

    return_statement = return_statement_alloc(parser->cur_token);
    next_token(parser);
    return_statement->return_value = parse_expression(parser, LOWEST_PREC);
    if (peek_token_is(parser, SEMICOLON))
    {
        next_token(parser);
    }
    return return_statement;
}

static struct expression_statement *parse_expression_statement(struct parser *parser)
{
    struct expression_statement *expression_statement;

    expression_statement = expression_statement_alloc(parser->cur_token);
    expression_statement->expression = parse_expression(parser, LOWEST_PREC);
    if (peek_token_is(parser, SEMICOLON))
    {
        next_token(parser);
    }
    return expression_statement;
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
        return (struct statement *) parse_expression_statement(parser);
    }
    }
}

static struct block_statement *parse_block_statement(struct parser *parser)
{
    struct block_statement *block_statement;
    struct statement *statement;

    block_statement = block_statement_alloc(parser->cur_token);
    next_token(parser);
    while (!cur_token_is(parser, RBRACE) && !cur_token_is(parser, EOF))
    {
        statement = parse_statement(parser);
        if (statement != NULL)
        {
            block_statement_append_statement(block_statement, statement);
        }        
        next_token(parser);
    }
    return block_statement;
}

static Seq_T parse_function_parameters(struct parser *parser)
{
    Seq_T parameters;
    struct identifier *identifier;
    bool success = false;

    parameters = Seq_new(10);
    if (peek_token_is(parser, RPAREN))
    {
        next_token(parser);
        success = true;
        goto cleanup;
    }
    next_token(parser);
    identifier = parse_identifier(parser);
    Seq_addhi(parameters, identifier);
    while (peek_token_is(parser, COMMA))
    {
        next_token(parser);
        next_token(parser);
        identifier = parse_identifier(parser);
        Seq_addhi(parameters, identifier);
    }
    if (!expect_peek(parser, RPAREN))
    {
        goto cleanup;
    }
    success = true;

cleanup:
    if (!success)
    {
        for (int i = 0; i < Seq_length(parameters); i++)
        {
            identifier = (struct identifier *) Seq_get(parameters, i);
            identifier_destroy(identifier);
        }
        Seq_free(&parameters);
        parameters = NULL;
    }
    return parameters;
}

static struct expression *parse_grouped_expression(struct parser *parser)
{
    struct expression *expression;
    
    next_token(parser);
    expression = parse_expression(parser, LOWEST_PREC);
    if (!expect_peek(parser, RPAREN))
    {
        expression_destroy(expression);
        return NULL;
    }
    return expression;
}

static struct expression *parse_prefix_expression(struct parser *parser)
{
    struct prefix_expression *prefix_expression;

    prefix_expression = prefix_expression_alloc(parser->cur_token);
    next_token(parser);
    prefix_expression->right = parse_expression(parser, PREFIX_PREC);
    return (struct expression *) prefix_expression;
}

static struct expression *parse_infix_expression(struct parser *parser,
    struct expression *left)
{
    struct infix_expression *infix_expression;
    enum precedence_type precedence;

    infix_expression = infix_expression_alloc(parser->cur_token);
    infix_expression->left = left;
    precedence = cur_precedence(parser);
    next_token(parser);
    infix_expression->right = parse_expression(parser, precedence);
    return (struct expression *) infix_expression;
}

static struct expression *parse_if_expression(struct parser *parser)
{
    struct if_expression *if_expression;
    bool success = false;

    if_expression = if_expression_alloc(parser->cur_token);
    if (!expect_peek(parser, LPAREN))
    {
        goto cleanup;
    }
    next_token(parser);
    if_expression->condition = parse_expression(parser, LOWEST_PREC);
    if (!expect_peek(parser, RPAREN))
    {
        goto cleanup;
    }
    if (!expect_peek(parser, LBRACE))
    {
        goto cleanup;
    }
    if_expression->consequence = parse_block_statement(parser);
    if (peek_token_is(parser, ELZE))
    {
        next_token(parser);
        if (!expect_peek(parser, LBRACE))
        {
            goto cleanup;
        }
        if_expression->alternative = parse_block_statement(parser);
    }
    success = true;

cleanup:
    if (!success)
    {
        if_expression_destroy(if_expression);
        if_expression = NULL;
    }
    return (struct expression *) if_expression;
}

static struct expression *parse_function_literal(struct parser *parser)
{
    struct function_literal *function_literal;
    bool success = false;

    function_literal = function_literal_alloc(parser->cur_token);
    if (!expect_peek(parser, LPAREN))
    {
        goto cleanup;
    }
    function_literal->parameters = parse_function_parameters(parser);
    if (!expect_peek(parser, LBRACE))
    {
        goto cleanup;
    }
    function_literal->body = parse_block_statement(parser);
    success = true;

cleanup:
    if (!success)
    {
        function_literal_destroy(function_literal);
        function_literal = NULL;
    }
    return (struct expression *) function_literal;
}

static Seq_T parse_expression_list(struct parser *parser, enum token_type end)
{
    Seq_T expressions;
    struct expression *expression;
    bool success = false;

    expressions = Seq_new(10);
    if (peek_token_is(parser, end))
    {
        next_token(parser);
        success = true;
        goto cleanup;
    }
    next_token(parser);
    expression = parse_expression(parser, LOWEST_PREC);
    Seq_addhi(expressions, expression);
    while (peek_token_is(parser, COMMA))
    {
        next_token(parser);
        next_token(parser);
        expression = parse_expression(parser, LOWEST_PREC);
        Seq_addhi(expressions, expression);
    }
    if (!expect_peek(parser, end))
    {
        goto cleanup;
    }
    success = true;

cleanup:
    if (!success)
    {
        for (int i = 0; i < Seq_length(expressions); i++)
        {
            expression = (struct expression *) Seq_get(expressions, i);
            expression_destroy(expression);
        }
        Seq_free(&expressions);
        expressions = NULL;
    }
    return expressions;
}

static struct expression *parse_call_expression(struct parser *parser,
                                                struct expression *function)
{
    struct call_expression *call_expression;

    call_expression = call_expression_alloc(parser->cur_token);
    call_expression->function = function;
    call_expression->arguments = parse_expression_list(parser, RPAREN);
    return (struct expression *) call_expression;
}

static struct array_literal *parse_array_literal(struct parser *parser)
{
    struct array_literal *array_literal;

    array_literal = array_literal_alloc(parser->cur_token);
    array_literal->elements = parse_expression_list(parser, RBRAKET);
    return array_literal;
}

static struct hash_literal *parse_hash_literal(struct parser *parser)
{
    struct hash_literal *hash_literal;
    struct expression *expression;
    bool success = false;

    hash_literal = hash_literal_alloc(parser->cur_token);
    hash_literal->keys = Seq_new(10);
    hash_literal->values  = Seq_new(10);
    while (!peek_token_is(parser, RBRACE))
    {
        next_token(parser);
        expression = parse_expression(parser, LOWEST_PREC);
        if (expression == NULL)
        {
            goto cleanup;
        }
        Seq_addhi(hash_literal->keys, expression);
        if (!expect_peek(parser, COLON))
        {
            goto cleanup;
        }
        next_token(parser);
        expression = parse_expression(parser, LOWEST_PREC);
        if (expression == NULL)
        {
            goto cleanup;
        }
        Seq_addhi(hash_literal->values, expression);
        if (!peek_token_is(parser, RBRACE) && !expect_peek(parser, COMMA))
        {
            goto cleanup;
        }
    }
    if (!expect_peek(parser, RBRACE))
    {
        goto cleanup;
    }
    success = true;

cleanup:
    if (!success)
    {
        hash_literal_destroy(hash_literal);
        hash_literal = NULL;
    }
    return hash_literal;
}

static struct expression *parse_index_expression(struct parser *parser,
                                                 struct expression *left)
{
    struct index_expression *index_expression;

    index_expression = index_expression_alloc(parser->cur_token);
    index_expression->left = left;
    next_token(parser);
    index_expression->index = parse_expression(parser, LOWEST_PREC);
    if (!expect_peek(parser, RBRAKET))
    {
        index_expression_destroy(index_expression);
        index_expression = NULL;
    }
    return (struct expression *) index_expression;
}

void parser_init(void)
{
    static struct type_precedence
    {
        enum token_type type;
        enum precedence_type precedence;
    } type_precedences[] =
      {
          { EQ, EQUALS_PREC },
          { NOT_EQ, EQUALS_PREC },
          { LT, LESSGREATER_PREC },
          { GT, LESSGREATER_PREC },
          { PLUS, SUM_PREC },
          { MINUS, SUM_PREC },
          { SLASH, PRODUCT_PREC },
          { ASTERISK, PRODUCT_PREC },
          { LPAREN, CALL_PREC },
          { LBRAKET, INDEX_PREC }
      };
    static struct type_prefix
    {
        enum token_type type;
        struct prefix_parse_fn prefix_parse_fn;
    } type_prefixes[] =
      {
          {IDENT, {(struct expression *(*)(struct parser *)) parse_identifier}},
          {STRING, {(struct expression *(*)(struct parser *)) parse_string_literal}},
          {INT, {(struct expression *(*)(struct parser *)) parse_integer_literal}},
          {LBRAKET, {(struct expression *(*)(struct parser *)) parse_array_literal}},
          {LBRACE, {(struct expression *(*)(struct parser *)) parse_hash_literal}},
          {TRUE, {(struct expression *(*)(struct parser *)) parse_boolean}},
          {FALSE, {(struct expression *(*)(struct parser *)) parse_boolean}},
          {BANG, {parse_prefix_expression}},
          {MINUS, {parse_prefix_expression}},
          {LPAREN, {parse_grouped_expression}},
          {IF, {parse_if_expression}},
          {FUNCTION, {parse_function_literal}}
      };
    static struct type_infix
    {
        enum token_type type;
        struct infix_parse_fn infix_parse_fn;
    } type_infixes[] =
      {
          {PLUS, {parse_infix_expression}},          
          {MINUS, {parse_infix_expression}},
          {SLASH, {parse_infix_expression}},
          {ASTERISK, {parse_infix_expression}},
          {EQ, {parse_infix_expression}},
          {NOT_EQ, {parse_infix_expression}},
          {LT, {parse_infix_expression}},
          {GT, {parse_infix_expression}},
          {LPAREN, {parse_call_expression}},
          {LBRAKET, {parse_index_expression}}
      };
    Fmt_register('T', Text_fmt);
    precedences = Table_new(0, int_cmp, int_hash);
    prefix_parse_fns = Table_new(0, int_cmp, int_hash);
    infix_parse_fns = Table_new(0, int_cmp, int_hash);
    for (int i = 0; i < sizeof type_precedences / sizeof type_precedences[0]; i++)
    {
        Table_put(precedences, &type_precedences[i].type,
                  (void *) type_precedences[i].precedence);
    }
    for (int i = 0; i < sizeof type_prefixes / sizeof type_prefixes[0]; i++)
    {
        Table_put(prefix_parse_fns, &type_prefixes[i].type,
                  &type_prefixes[i].prefix_parse_fn);
    }
    for (int i = 0; i < sizeof type_infixes / sizeof type_infixes[0]; i++)
    {
        Table_put(infix_parse_fns, &type_infixes[i].type,
                  &type_infixes[i].infix_parse_fn);
    }
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
