#include <mem.h>
#include <str.h>

#include "ast.h"

static Text_T empty = { 0, "" };

const char *node_type_str [] =
{
    [PROGRAM] = "PROGRAM",
    [LET_STMT] = "LET STMT",
    [RETURN_STMT] = "RETURN STMT",
    [EXPR_STMT] = "EXPR STMT",
    [BLOCK_STMT] = "BLOCK STMT",
    [IDENT_EXPR] = "IDENT EXPR",
    [STRING_LITERAL_EXPR] = "STRING_LITERAL",
    [INT_LITERAL_EXPR] = "INT_LITERAL",
    [ARRAY_LITERAL_EXPR] = "ARRAY_LITERAL",
    [HASH_LITERAL_EXPR] = "HASH_LITERAL",
    [FUNC_LITERAL_EXPR] = "FUNC_LITERAL",
    [INDEX_EXPR] = "INDEX EXPR",
    [PREFIX_EXPR] = "PREFIX EXPR",
    [INFIX_EXPR] = "INFIX EXPR",
    [BOOL_EXPR] = "BOOL EXPR",
    [IF_EXPR] = "IF EXPR",
    [CALL_EXPR] = "CALL EXPR"
};

Text_T expression_token_literal(struct expression *expression)
{
    
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        return identifier_token_literal((struct identifier *) expression);
    }
    case STRING_LITERAL_EXPR:
    {
        return string_literal_token_literal((struct string_literal *) expression);
    }
    case INT_LITERAL_EXPR:
    {
        return integer_literal_token_literal((struct integer_literal *) expression);
    }
    case ARRAY_LITERAL_EXPR:
    {
        return array_literal_token_literal((struct array_literal *) expression);
    }
    case HASH_LITERAL_EXPR:
    {
        return hash_literal_token_literal((struct hash_literal *) expression);
    }
    case FUNC_LITERAL_EXPR:
    {
        return function_literal_token_literal((struct function_literal *) expression);
    }
    case BOOL_EXPR:
    {
        return boolean_token_literal((struct boolean *) expression);
    }
    case INDEX_EXPR:
    {
        return prefix_expression_token_literal((struct prefix_expression *) expression);
    }
    case PREFIX_EXPR:
    {
        return prefix_expression_token_literal((struct prefix_expression *) expression);
    }
    case INFIX_EXPR:
    {
        return infix_expression_token_literal((struct infix_expression *) expression);
    }
    case IF_EXPR:
    {
        return if_expression_token_literal((struct if_expression *) expression);
    }
    case CALL_EXPR:
    {
        return call_expression_token_literal((struct call_expression *) expression);
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
    case EXPR_STMT:
    {
        return expression_statement_token_literal((struct expression_statement *) statement);
    }
    case BLOCK_STMT:
    {
        return block_statement_token_literal((struct block_statement *) statement);
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

Text_T string_literal_token_literal(struct string_literal *string_literal)
{
    return string_literal->token.literal;
}

Text_T integer_literal_token_literal(struct integer_literal *integer_literal)
{
    return integer_literal->token.literal;
}

Text_T array_literal_token_literal(struct array_literal *array_literal)
{
    return array_literal->token.literal;
}

Text_T hash_literal_token_literal(struct hash_literal *hash_literal)
{
    return hash_literal->token.literal;
}

Text_T function_literal_token_literal(struct function_literal *function_literal)
{
    return function_literal->token.literal;
}

Text_T boolean_token_literal(struct boolean *boolean)
{
    return boolean->token.literal;
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

Text_T block_statement_token_literal(struct block_statement *block_statement)
{
    return block_statement->token.literal;
}

Text_T index_expression_token_literal(struct index_expression *index_expression)
{
    return index_expression->token.literal;
}

Text_T prefix_expression_token_literal(struct prefix_expression *prefix_expression)
{
    return prefix_expression->token.literal;
}

Text_T infix_expression_token_literal(struct infix_expression *infix_expression)
{
    return infix_expression->token.literal;
}

Text_T if_expression_token_literal(struct if_expression *if_expression)
{
    return if_expression->token.literal;
}

Text_T call_expression_token_literal(struct call_expression *call_expression)
{
    return call_expression->token.literal;
}

char *expression_to_string(struct expression *expression)
{
    
    switch (expression->type)
    {
    case IDENT_EXPR:
    {
        return identifier_to_string((struct identifier *) expression);
    }
    case STRING_LITERAL_EXPR:
    {
        return string_literal_to_string((struct string_literal *) expression);
    }
    case INT_LITERAL_EXPR:
    {
        return integer_literal_to_string((struct integer_literal *) expression);
    }
    case ARRAY_LITERAL_EXPR:
    {
        return array_literal_to_string((struct array_literal *) expression);
    }
    case HASH_LITERAL_EXPR:
    {
        return hash_literal_to_string((struct hash_literal *) expression);
    }
    case FUNC_LITERAL_EXPR:
    {
        return function_literal_to_string((struct function_literal *) expression);
    }
    case BOOL_EXPR:
    {
        return boolean_to_string((struct boolean *) expression);
    }
    case INDEX_EXPR:
    {
        return index_expression_to_string((struct index_expression *) expression);
    }
    case PREFIX_EXPR:
    {
        return prefix_expression_to_string((struct prefix_expression *) expression);
    }
    case INFIX_EXPR:
    {
        return infix_expression_to_string((struct infix_expression *) expression);
    }
    case IF_EXPR:
    {
        return if_expression_to_string((struct if_expression *) expression);
    }
    case CALL_EXPR:
    {
        return call_expression_to_string((struct call_expression *) expression);
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
    case BLOCK_STMT:
    {
        return block_statement_to_string((struct block_statement *) statement);
    }
    default:
    {
        return NULL;
    }
    }
}

char *program_to_string(struct program *program)
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
    Fmt_sfmt(str, len, "%T", &identifier->token.literal);
    return str;
}

char *string_literal_to_string(struct string_literal *string_literal)
{
    int len;
    char *str;

    len = string_literal->token.literal.len + 1;
    str = ALLOC(len);
    Fmt_sfmt(str, len, "%T", &string_literal->token.literal);
    return str;
}

char *integer_literal_to_string(struct integer_literal *integer_literal)
{
    int len;
    char *str;

    len = 24;
    str = ALLOC(len);
    snprintf(str, len, "%lld", integer_literal->value);
    return str;
}

char *array_literal_to_string(struct array_literal *array_literal)
{
    struct expression *expression;
    char *str;
    char *str1;
    char *str2;
    
    str1 = Str_dup("[", 1, 0, 1);
    if (Seq_length(array_literal->elements) > 0)
    {
        expression = (struct expression *) Seq_get(array_literal->elements, 0);
        str2 = expression_to_string(expression);
        str = Str_cat(str1, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        str1 = str;
        for (int i = 1; i < Seq_length(array_literal->elements); i++)
        {
            expression = (struct expression *) Seq_get(array_literal->elements, i);
            str2 = expression_to_string(expression);
            str = Str_catv(str1, 1, 0, ", ", 1, 0, str2, 1, 0, NULL);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, "]", 1, 0);
    FREE(str1);
    return str;
}

char *hash_literal_to_string(struct hash_literal *hash_literal)
{
    struct expression *expression;
    char *str;
    char *str1;
    char *str2;
    char *str3;
    
    str1 = Str_dup("{", 1, 0, 1);
    if (Seq_length(hash_literal->keys) > 0)
    {
        expression = (struct expression *) Seq_get(hash_literal->keys, 0);
        str2 = expression_to_string(expression);
        expression = (struct expression *) Seq_get(hash_literal->values, 0);
        str3 = expression_to_string(expression);
        str = Str_catv(str1, 1, 0, str2, 1, 0, " : ", 1, 0, str3, 1, 0, NULL);
        FREE(str3);
        FREE(str2);
        FREE(str1);
        str1 = str;
        for (int i = 1; i < Seq_length(hash_literal->keys); i++)
        {
            expression = (struct expression *) Seq_get(hash_literal->keys, i);
            str2 = expression_to_string(expression);
            expression = (struct expression *) Seq_get(hash_literal->values, 0);
            str3 = expression_to_string(expression);
            str = Str_catv(str1, 1, 0, ", ", 1, 0, str2, 1, 0, " : ", 1, 0, str3, 1, 0);
            FREE(str3);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, "}", 1, 0);
    FREE(str1);
    return str;
}

char *function_literal_to_string(struct function_literal *function_literal)
{
    struct identifier *identifier;
    int len;
    char *str;
    char *str1;
    char *str2;

    len = function_literal->token.literal.len + 2;
    str = ALLOC(len);
    str1 = str;
    Fmt_sfmt(str1, len, "%T(", &function_literal->token.literal);
    if (Seq_length(function_literal->parameters) > 0)
    {
        identifier = (struct identifier *) Seq_get(function_literal->parameters, 0);
        str2 = identifier_to_string(identifier);
        str = Str_cat(str, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        for (int i = 1; i < Seq_length(function_literal->parameters); i++)
        {
            identifier = (struct identifier *) Seq_get(function_literal->parameters, i);
            str2 = identifier_to_string(identifier);
            str = Str_catv(str, 1, 0, ", ", 1, 0, str2, 1, 0, NULL);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, ")", 1, 0);
    FREE(str1);
    str1 = str;
    str2 = block_statement_to_string(function_literal->body);
    str = Str_cat(str1, 1, 0, str2, 1, 0);
    FREE(str2);
    FREE(str1);
    return str;
}

char *boolean_to_string(struct boolean *boolean)
{
    int len;
    char *str;

    len = boolean->token.literal.len + 1;
    str = ALLOC(len);
    Fmt_sfmt(str, len, "%T", &boolean->token.literal);
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
    str = Str_catv(str1, 1, 0, str2, 1, 0, " = ", 1, 0, NULL);
    FREE(str2);
    FREE(str1);
    str1 = str;
    if (let_statement->value != NULL)
    {
        // TODO: fix
        str2 = expression_to_string(let_statement->value);
        str = Str_cat(str1, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        str1 = str;
    }
    str = Str_cat(str1, 1, 0, ";", 1, 0);
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
        // TODO: fix
        str2 = expression_to_string(return_statement->return_value);
        str = Str_cat(str1, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        str1 = str;
    }
    str = Str_cat(str1, 1, 0, ";", 1, 0);
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

char *block_statement_to_string(struct block_statement *block_statement)
{
    struct statement *statement;
    char *str;
    char *str1;
    char *str2;
        
    if (Seq_length(block_statement->statements) > 0)
    {
        statement = (struct statement *) Seq_get(block_statement->statements, 0);
        str = statement_to_string(statement);
        str1 = str;
        for (int i = 1; i < Seq_length(block_statement->statements); i++)
        {
            statement = (struct statement *) Seq_get(block_statement->statements, i);
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

char *index_expression_to_string(struct index_expression *index_expression)
{
    char *str;
    char *str1;
    char *str2;

    str1 = expression_to_string(index_expression->left);
    str2 = expression_to_string(index_expression->index);
    str = Str_catv("(", 1, 0, str1, 1, 0, "[", 1, 0, str2, 1, 0, "])", 1, 0, NULL);
    FREE(str2);
    FREE(str1);
    return str;
}

char *prefix_expression_to_string(struct prefix_expression *prefix_expression)
{
    int len;
    char *str;
    char *str1;
    char *str2;

    len = prefix_expression->op.len + 2;
    str1 = ALLOC(len);
    Fmt_sfmt(str1, len, "(%T", &prefix_expression->op);
    str2 = expression_to_string(prefix_expression->right);
    str = Str_catv(str1, 1, 0, str2, 1, 0, ")", 1, 0, NULL);
    FREE(str2);
    FREE(str1);
    return str;
}

char *infix_expression_to_string(struct infix_expression *infix_expression)
{
    int len;
    char *str;
    char *str1;
    char *str2;
    char *str3;

    str1 = expression_to_string(infix_expression->left);
    len = infix_expression->op.len + 3;
    str2 = ALLOC(len);
    Fmt_sfmt(str2, len, " %T ", &infix_expression->op);
    str3 = expression_to_string(infix_expression->right);
    str = Str_catv("(", 1, 0, str1, 1, 0, str2, 1, 0, str3, 1, 0, ")", 1, 0, NULL);
    FREE(str3);
    FREE(str2);
    FREE(str1);
    return str;
}

char *if_expression_to_string(struct if_expression *if_expression)
{
    char *str;
    char *str1;
    char *str2;

    str1 = expression_to_string(if_expression->condition);
    str2 = block_statement_to_string(if_expression->consequence);
    str = Str_catv("if", 1, 0, str1, 1, 0, " ", 1, 0, str2, 1, 0, NULL);
    FREE(str2);
    FREE(str1);
    if (if_expression->alternative != NULL)
    {
        str1 = block_statement_to_string(if_expression->alternative);
        str2 = str;
        str = Str_catv(str1, 1, 0, "else", 1, 0, str2, 1, 0, NULL);
        FREE(str2);
        FREE(str1);
    }
    return str;
}

char *call_expression_to_string(struct call_expression *call_expression)
{
    struct expression *expression;
    char *str;
    char *str1;
    char *str2;

    str = expression_to_string(call_expression->function);
    str1 = str;
    str = Str_cat(str, 1, 0, "(", 1, 0);
    FREE(str1);
    str1 = str;
    if (Seq_length(call_expression->arguments) > 0)
    {
        expression = (struct expression *) Seq_get(call_expression->arguments, 0);
        str2 = expression_to_string(expression);
        str = Str_cat(str1, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        str1 = str;
        for (int i = 1; i < Seq_length(call_expression->arguments); i++)
        {
            expression = (struct expression *) Seq_get(call_expression->arguments, i);
            str2 = expression_to_string(expression);
            str = Str_catv(str1, 1, 0, ", ", 1, 0, str2, 1, 0, NULL);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, ")", 1, 0);
    FREE(str1);
    return str;
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
    case STRING_LITERAL_EXPR:
    {
        string_literal_destroy((struct string_literal *) expression);
        break;
    }
    case INT_LITERAL_EXPR:
    {
        integer_literal_destroy((struct integer_literal *) expression);
        break;
    }
    case ARRAY_LITERAL_EXPR:
    {
        array_literal_destroy((struct array_literal *) expression);
        break;
    }
    case HASH_LITERAL_EXPR:
    {
        hash_literal_destroy((struct hash_literal *) expression);
        break;
    }
    case FUNC_LITERAL_EXPR:
    {
        function_literal_destroy((struct function_literal *) expression);
        break;
    }
    case BOOL_EXPR:
    {
        boolean_destroy((struct boolean *) expression);
        break;
    }
    case INDEX_EXPR:
    {
        index_expression_destroy((struct index_expression *) expression);
        break;
    }
    case PREFIX_EXPR:
    {
        prefix_expression_destroy((struct prefix_expression *) expression);
        break;
    }
    case INFIX_EXPR:
    {
        infix_expression_destroy((struct infix_expression *) expression);
        break;
    }
    case IF_EXPR:
    {
        if_expression_destroy((struct if_expression *) expression);
        break;
    }
    case CALL_EXPR:
    {
        call_expression_destroy((struct call_expression *) expression);
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
    case EXPR_STMT:
    {
        expression_statement_destroy((struct expression_statement *) statement);
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
    program->type = PROGRAM;
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

void block_statement_append_statement(struct block_statement *block_statement,
                                      struct statement *statement)
{
    Seq_addhi(block_statement->statements, statement);
}

struct let_statement *let_statement_alloc(struct token token)
{
    struct let_statement *let_statement;

    NEW0(let_statement);
    let_statement->type = LET_STMT;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    let_statement->token = token;
    return let_statement;
}

struct return_statement *return_statement_alloc(struct token token)
{
    struct return_statement *return_statement;

    NEW0(return_statement);
    return_statement->type = RETURN_STMT;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    return_statement->token = token;
    return return_statement;
}

struct identifier *identifier_alloc(struct token token)
{
    struct identifier *identifier;

    NEW0(identifier);
    identifier->type = IDENT_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    identifier->token = token;
    return identifier;
}

struct string_literal *string_literal_alloc(struct token token)
{
    struct string_literal *string_literal;

    NEW0(string_literal);
    string_literal->type = STRING_LITERAL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    string_literal->token = token;
    return string_literal;
}

struct integer_literal *integer_literal_alloc(struct token token)
{
    struct integer_literal *integer_literal;

    NEW0(integer_literal);
    integer_literal->type = INT_LITERAL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    integer_literal->token = token;
    return integer_literal;
}

struct array_literal *array_literal_alloc(struct token token)
{
    struct array_literal *array_literal;

    NEW0(array_literal);
    array_literal->type = ARRAY_LITERAL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    array_literal->token = token;
    return array_literal;
}

struct hash_literal *hash_literal_alloc(struct token token)
{
    struct hash_literal *hash_literal;

    NEW0(hash_literal);
    hash_literal->type = HASH_LITERAL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    hash_literal->token = token;
    return hash_literal;
}

struct function_literal *function_literal_alloc(struct token token)
{
    struct function_literal *function_literal;

    NEW0(function_literal);
    function_literal->type = FUNC_LITERAL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    function_literal->token = token;
    function_literal->cnt = 1;
    return function_literal;
}

void function_literal_addref(struct function_literal *function_literal)
{
    function_literal->cnt++;
}

struct boolean *boolean_alloc(struct token token, bool value)
{
    struct boolean *boolean;

    NEW0(boolean);
    boolean->type = BOOL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    boolean->token = token;
    boolean->value = value;
    return boolean;
}

struct expression_statement *expression_statement_alloc(struct token token)
{
    struct expression_statement *expression_statement;

    NEW0(expression_statement);
    expression_statement->type = EXPR_STMT;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    expression_statement->token = token;
    return expression_statement;
}

struct block_statement *block_statement_alloc(struct token token)
{
    struct block_statement *block_statement;

    NEW0(block_statement);
    block_statement->type = BLOCK_STMT;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    block_statement->token = token;
    block_statement->statements = Seq_new(100);
    return block_statement;
}

struct index_expression *index_expression_alloc(struct token token)
{
    struct index_expression *index_expression;

    NEW0(index_expression);
    index_expression->type = INDEX_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    index_expression->token = token;
    return index_expression;
}

struct prefix_expression *prefix_expression_alloc(struct token token)
{
    struct prefix_expression *prefix_expression;

    NEW0(prefix_expression);
    prefix_expression->type = PREFIX_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    prefix_expression->token = token;
    prefix_expression->op = token.literal;
    return prefix_expression;
}

struct infix_expression *infix_expression_alloc(struct token token)
{
    struct infix_expression *infix_expression;

    NEW0(infix_expression);
    infix_expression->type = INFIX_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    infix_expression->token = token;
    infix_expression->op = token.literal;
    return infix_expression;
}

struct if_expression *if_expression_alloc(struct token token)
{
    struct if_expression *if_expression;

    NEW0(if_expression);
    if_expression->type = IF_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    if_expression->token = token;
    return if_expression;
}

struct call_expression *call_expression_alloc(struct token token)
{
    struct call_expression *call_expression;

    NEW0(call_expression);
    call_expression->type = CALL_EXPR;
    token.literal = Text_box(Text_get(NULL, 0, token.literal), token.literal.len);
    call_expression->token = token;
    return call_expression;
}

void let_statement_destroy(struct let_statement *let_statement)
{
    char *c;
    
    if (let_statement == NULL)
    {
        return;
    }
    identifier_destroy(let_statement->name);
    expression_destroy(let_statement->value);
    c = (char *) let_statement->token.literal.str;
    FREE(c);
    FREE(let_statement);
}

void return_statement_destroy(struct return_statement *return_statement)
{
    char *c;
        
    if (return_statement == NULL)
    {
        return;
    }
    expression_destroy(return_statement->return_value);
    c = (char *) return_statement->token.literal.str;
    FREE(c);
    FREE(return_statement);
}

void expression_statement_destroy(struct expression_statement *expression_statement)
{
    char *c;
    
    if (expression_statement == NULL)
    {
        return;
    }
    expression_destroy(expression_statement->expression);
    c = (char *) expression_statement->token.literal.str;
    FREE(c);
    FREE(expression_statement);
}

void block_statement_destroy(struct block_statement *block_statement)
{
    struct statement *statement;
    char *c;

    if (block_statement == NULL)
    {
        return;
    }
    for (int i = 0; i < Seq_length(block_statement->statements); i++)
    {
        statement = (struct statement *) Seq_get(block_statement->statements, i);
        statement_destroy(statement);
    }
    Seq_free(&block_statement->statements);
    c = (char *) block_statement->token.literal.str;
    FREE(c);
    FREE(block_statement);
}

void index_expression_destroy(struct index_expression *index_expression)
{
    char *c;
    
    if (index_expression == NULL)
    {
        return;
    }
    expression_destroy(index_expression->left);
    expression_destroy(index_expression->index);
    c = (char *) index_expression->token.literal.str;
    FREE(c);
    FREE(index_expression);
}

void prefix_expression_destroy(struct prefix_expression *prefix_expression)
{
    char *c;
    
    if (prefix_expression == NULL)
    {
        return;
    }
    expression_destroy(prefix_expression->right);
    c = (char *) prefix_expression->token.literal.str;
    FREE(c);
    FREE(prefix_expression);
}

void infix_expression_destroy(struct infix_expression *infix_expression)
{
    char *c;
    
    if (infix_expression == NULL)
    {
        return;
    }
    expression_destroy(infix_expression->left);
    expression_destroy(infix_expression->right);
    c = (char *) infix_expression->token.literal.str;
    FREE(c);
    FREE(infix_expression);
}

void if_expression_destroy(struct if_expression *if_expression)
{
    char *c;
    
    if (if_expression == NULL)
    {
        return;
    }
    expression_destroy(if_expression->condition);
    block_statement_destroy(if_expression->consequence);
    block_statement_destroy(if_expression->alternative);
    c = (char *) if_expression->token.literal.str;
    FREE(c);
    FREE(if_expression);
}

void call_expression_destroy(struct call_expression *call_expression)
{
    struct expression *expression;
    char *c;
        
    if (call_expression == NULL)
    {
        return;
    }
    expression_destroy(call_expression->function);
    if (call_expression->arguments != NULL)
    {
        for (int i = 0; i < Seq_length(call_expression->arguments); i++)
        {
            expression = (struct expression *) Seq_get(call_expression->arguments, i);
            expression_destroy(expression);
        }
        Seq_free(&call_expression->arguments);
    }
    c = (char *) call_expression->token.literal.str;
    FREE(c);
    FREE(call_expression);
}

void identifier_destroy(struct identifier *identifier)
{
    char *c;

    c = (char *) identifier->value.str;
    FREE(c);
    c = (char *) identifier->token.literal.str;
    FREE(c);
    FREE(identifier);
}

void string_literal_destroy(struct string_literal *string_literal)
{
    char *c;

    c = (char *) string_literal->value.str;
    FREE(c);
    c = (char *) string_literal->token.literal.str;
    FREE(c);
    FREE(string_literal);
}

void integer_literal_destroy(struct integer_literal *integer_literal)
{
    char *c;
    
    c = (char *) integer_literal->token.literal.str;
    FREE(c);
    FREE(integer_literal);
}

void array_literal_destroy(struct array_literal *array_literal)
{
    struct expression *expression;
    char *c;
        
    if (array_literal == NULL)
    {
        return;
    }
    if (array_literal->elements != NULL)
    {
        for (int i = 0; i < Seq_length(array_literal->elements); i++)
        {
            expression = (struct expression *) Seq_get(array_literal->elements, i);
            expression_destroy(expression);
        }
        Seq_free(&array_literal->elements);
    }
    c = (char *) array_literal->token.literal.str;
    FREE(c);
    FREE(array_literal);
}

void hash_literal_destroy(struct hash_literal *hash_literal)
{
    struct expression *expression;
    char *c;
        
    if (hash_literal == NULL)
    {
        return;
    }
    if (hash_literal->keys != NULL)
    {
        for (int i = 0; i < Seq_length(hash_literal->keys); i++)
        {
            expression = (struct expression *) Seq_get(hash_literal->keys, i);
            expression_destroy(expression);
            expression = (struct expression *) Seq_get(hash_literal->values, i);
            expression_destroy(expression);
        }
        Seq_free(&hash_literal->keys);
        Seq_free(&hash_literal->values);
    }
    c = (char *) hash_literal->token.literal.str;
    FREE(c);
    FREE(hash_literal);
}

void function_literal_destroy(struct function_literal *function_literal)
{
    struct identifier *identifier;
    char *c;

    if (function_literal == NULL)
    {
        return;
    }
    if (--function_literal->cnt > 0)
    {
        return;
    }
    if (function_literal->parameters != NULL)
    {
        for (int i = 0; i < Seq_length(function_literal->parameters); i++)
        {
            identifier = (struct identifier *) Seq_get(function_literal->parameters, i);
            identifier_destroy(identifier);
        }
        Seq_free(&function_literal->parameters);
    }
    block_statement_destroy(function_literal->body);
    c = (char *) function_literal->token.literal.str;
    FREE(c);
    FREE(function_literal);
}

void boolean_destroy(struct boolean *boolean)
{
    char *c;
    
    c = (char *) boolean->token.literal.str;
    FREE(c);
    FREE(boolean);
}
