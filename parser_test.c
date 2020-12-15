#include <stdlib.h>
#include <stdio.h>
#include <str.h>
#include <mem.h>

#include "parser.h"

static int check_parse_errors(struct parser *parser)
{
    char *msg;
    
    if (Seq_length(parser->errors) == 0)
    {
        return 0;
    }
    Fmt_print("parser has %d errors\n", Seq_length(parser->errors));
    for (int i = 0; i < Seq_length(parser->errors); i++)
    {
        msg = (char *) Seq_get(parser->errors, i);
        Fmt_print("parser errors: %s\n", msg);
    }
    return -1;
}

static int test_integer_literal(struct expression *expression, long long value)
{
    struct integer_literal *integer_literal;
    char str[24];
    snprintf(str, sizeof str, "%lld", value);
    Text_T value_literal = Text_box(str, Str_len(str, 1, 0));

    if (expression->type != INT_LITERAL_EXPR)
    {
        Fmt_print("not integer_literal got=%d\n",
                  expression->type);
        return -1;
    }
    integer_literal = (struct integer_literal *) expression;
    if (integer_literal->value != value)
    {
        Fmt_print("integer_literal->value not '%lld', got=%lld\n", value,
                  integer_literal->value);
        return -1;
    }
    if (Text_cmp(integer_literal_token_literal(integer_literal), value_literal) != 0)
    {
        Text_T t = integer_literal_token_literal(integer_literal);
        Fmt_print("token_literal not '%T', got=%T\n", &value_literal, &t);
        return -1;
    }
    return 0;
}

static int test_identifier(struct expression *expression, const char *value)
{
    struct identifier *identifier;
    Text_T value_literal = Text_box(value, Str_len(value, 1, 0));
    
    if (expression->type != IDENT_EXPR)
    {
        Fmt_print("not identifier got=%d\n",
                  expression->type);
        return -1;
    }
    identifier = (struct identifier *) expression;
    if (Text_cmp(identifier->value, value_literal) != 0)
    {
        Fmt_print("identifier->value not '%s', got=%T\n", value,
                  &identifier->value);
        return -1;
    }
    if (Text_cmp(identifier_token_literal(identifier), value_literal) != 0)
    {
        Text_T t = identifier_token_literal(identifier);
        Fmt_print("token_literal not '%T', got=%T\n", &value, &t);
        return -1;
    }
    return 0;
}

static int test_boolean_literal(struct expression *expression, bool value)
{
    struct boolean  *boolean;
    char str[24];
    snprintf(str, sizeof str, "%s", value ? "true" : "false");
    Text_T value_literal = Text_box(str, Str_len(str, 1, 0));

    if (expression->type != BOOL_EXPR)
    {
        Fmt_print("not boolean got=%d\n", expression->type);
        return -1;
    }
    boolean = (struct boolean *) expression;
    if (boolean->value != value)
    {
        Fmt_print("boolean->value not '%s', got=%s\n",
                  value ? "true" : "false",
                  boolean->value ? "true" : "false");
        return -1;
    }
    if (Text_cmp(boolean_token_literal(boolean), value_literal) != 0)
    {
        Text_T t = boolean_token_literal(boolean);
        Fmt_print("token_literal not '%T', got=%T\n", &value_literal, &t);
        return -1;
    }
    return 0;
}

static int test_identifier_expression(void)
{
    const char *input = "foobar;";
    const char *value = "foobar";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statement, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != IDENT_EXPR)
    {
        Fmt_print("not identifier got=%d\n",
                  expression_statement->expression->type);
        goto cleanup;
    }
    if (test_identifier(expression_statement->expression, value) != 0)
    {
        goto cleanup;
    }
    success = 0;
        
cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    lexer_destroy(lexer);
    parser_destroy(parser);
    return success;
}

static int test_string_literal_expression(void)
{
    const char *input = "\"hello world\";";
    Text_T expected = { sizeof "hello world" - 1, "hello world"};
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct string_literal *string_literal = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statement, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != STRING_LITERAL_EXPR)
    {
        Fmt_print("not string_literal got=%d\n",
                  expression_statement->expression->type);
        goto cleanup;
    }
    string_literal = (struct string_literal *) expression_statement->expression;
    if (Text_cmp(string_literal->value, expected) != 0)
    {
        Fmt_print("string_literal->value not '%T', got=%T\n", &expected, &string_literal->value);
        goto cleanup;
        return -1;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    lexer_destroy(lexer);
    parser_destroy(parser);
    return success;
}

static int test_integer_literal_expression(void)
{
    const char *input = "5;";
    int expected = 5;
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statement, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != INT_LITERAL_EXPR)
    {
        Fmt_print("not integer_literal got=%d\n",
                  expression_statement->expression->type);
        goto cleanup;
    }
    if (test_integer_literal(expression_statement->expression, expected) != 0)
    {
        goto cleanup;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    lexer_destroy(lexer);
    parser_destroy(parser);
    return success;
}

static int test_boolean_expression(void)
{
    struct test
    {
        const char *input;
        bool expected;
    } tests[] =
          {
              {"true", true },
              {"false", false }
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statement, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (statement->type != EXPR_STMT)
        {
            Fmt_print("not expression_statement got=%d\n", statement->type);
            goto cleanup;
        }
        expression_statement = (struct expression_statement *) statement;
        if (test_boolean_literal(expression_statement->expression, tests[i].expected))
        {
            goto cleanup;
        }
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    lexer_destroy(lexer);
    parser_destroy(parser);
    return success;
}

static int test_literal_expression(struct expression *expression, enum node_type type,
                                   void *value)
{
    switch (type)
    {
    case IDENT_EXPR:
    {
        return test_identifier(expression, (const char *) value);
    }
    case INT_LITERAL_EXPR:
    {
        return test_integer_literal(expression, (long long) value);
    }
    case BOOL_EXPR:
    {
        return test_boolean_literal(expression, (bool) value);
    }
    default:
    {
        Fmt_print("type of expr not handled got=%d\n", type);
        break;
    }
    }
    return -1;
}

static int test_let_statement(struct statement *s, const char *name,
                              enum node_type type, void *value)
{
    struct let_statement *let_statement;
    Text_T let = { sizeof "let" - 1, "let"};

    if (s->type != LET_STMT)
    {
        Fmt_print("not let_statement got=%d\n", s->type);
        return -1;
    }
    if (Text_cmp(statement_token_literal(s), let) != 0)

    {
        Text_T t = statement_token_literal(s);
        Fmt_print("token_literal not '%T', got=%T\n",
                  &let, &t);
        return -1;
    }
    let_statement = (struct let_statement *) s;
    if (test_identifier((struct expression *) let_statement->name, name) != 0)
    {
        return -1;
    }
    if (test_literal_expression(let_statement->value, type, value) != 0)
    {
        return -1;
    }
    return 0;
}

static int test_let_statements(void)
{
    struct test
    {
        const char *input;
        const char *name;
        enum node_type type;
        void *value;
    } tests[] =
          {
              {"let x = 5;", "x", INT_LITERAL_EXPR, (void *) 5},
              {"let y = true;", "y", BOOL_EXPR, (void *) true},
              {"let z = x;", "z",IDENT_EXPR, "x"}
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statements, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (test_let_statement(statement, tests[i].name,
                               tests[i].type, tests[i].value) != 0)
        {
            goto cleanup;
        }
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_return_statements(void)
{
    struct test
    {
        const char *input;
        enum node_type type;
        void *value;
    } tests[] =
          {
              {"return 5;", INT_LITERAL_EXPR, (void *) 5},
              {"return true;", BOOL_EXPR, (void *) true},
              {"return x;", IDENT_EXPR, "x"}
          };
    Text_T ret = { sizeof "return" - 1, "return"};
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct return_statement *return_statement = NULL;
    int success = -1;
    
    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statements, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (statement->type != RETURN_STMT)
        {
            Fmt_print("not return_statement got=%d\n", statement->type);
            goto cleanup;
        }
        return_statement = (struct return_statement *) statement;
        if (Text_cmp(statement_token_literal(statement), ret) != 0)
        {
            Text_T t = statement_token_literal(statement);
            Fmt_print("s.TokenLiteral not '%T', got=%T\n", &ret, &t);
            goto cleanup;
        }
        if (test_literal_expression(return_statement->return_value, tests[i].type, tests[i].value) != 0)
        {
            goto cleanup;
        }
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_prefix_expression(struct expression *expression,
                                  const char *operator,
                                  enum node_type type,
                                  void *value)
{
    struct prefix_expression *prefix_expression;
    Text_T operator_literal = Text_box(operator, Str_len(operator, 1, 0));
    
    if (expression->type != PREFIX_EXPR)
    {
        Fmt_print("not prefix_expression got=%d\n", expression->type);
        return -1;
    }
    prefix_expression = (struct prefix_expression *) expression;
    if (Text_cmp(prefix_expression->op, operator_literal) != 0)
    {
        Fmt_print("operator is not '%T'. got=%T\n", &operator_literal, &prefix_expression->op);
        return -1;
    }
    if (test_literal_expression(prefix_expression->right, type, value) != 0)
    {
        return -1;
    }
    return 0;
}

static int test_infix_expression(struct expression *expression,
                                 enum node_type ltype,
                                 void *lvalue,
                                 const char *operator,
                                 enum node_type rtype,
                                 void *rvalue)

{
    struct infix_expression *infix_expression;
    Text_T operator_literal = Text_box(operator, Str_len(operator, 1, 0));
    
    if (expression->type != INFIX_EXPR)
    {
        Fmt_print("not infix got=%d\n", expression->type);
        return -1;
    }
    infix_expression = (struct infix_expression *) expression;
    if (test_literal_expression(infix_expression->left, ltype, lvalue) != 0)
    {
        return -1;
    }
    if (Text_cmp(infix_expression->op, operator_literal) != 0)
    {
        Fmt_print("operator is not '%T'. got=%T\n", &operator_literal, &infix_expression->op);
        return -1;
    }
    if (test_literal_expression(infix_expression->right, rtype, rvalue) != 0)
    {
        return -1;
    }
    return 0;
}

static int test_prefix_expressions(void)
{
    struct test
    {
        const char *input;
        const char *operator;
        enum node_type type;
        void *value;
    } tests[] =
          {
              {"!5;", "!", INT_LITERAL_EXPR, (void *) 5},
              {"-15;", "-", INT_LITERAL_EXPR, (void *) 15},
              {"!true", "!", BOOL_EXPR, (void *) true},
              {"!false;", "!", BOOL_EXPR, (void *) false},
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statements, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (statement->type != EXPR_STMT)
        {
            Fmt_print("not expression_statement got=%d\n", statement->type);
            goto cleanup;
        }
        expression_statement = (struct expression_statement *) statement;
        if (test_prefix_expression(expression_statement->expression,
                                   tests[i].operator, tests[i].type,
                                   tests[i].value) != 0)
        {
            goto cleanup;
        }
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_infix_expressions(void)
{
    struct tests
    {
        const char *input;
        enum node_type ltype;
        void *lvalue;
        const char *operator;
        enum node_type rtype;
        void *rvalue;
    } tests[] =
          {
              {"5 + 5;", INT_LITERAL_EXPR, (void *) 5, "+", INT_LITERAL_EXPR, (void *) 5},
              {"5 - 5;", INT_LITERAL_EXPR, (void *) 5, "-", INT_LITERAL_EXPR, (void *) 5},
              {"5 * 5;", INT_LITERAL_EXPR, (void *) 5, "*", INT_LITERAL_EXPR, (void *) 5},
              {"5 / 5;", INT_LITERAL_EXPR, (void *) 5, "/", INT_LITERAL_EXPR, (void *) 5},
              {"5 > 5;", INT_LITERAL_EXPR, (void *) 5, ">", INT_LITERAL_EXPR, (void *) 5},
              {"5 < 5;", INT_LITERAL_EXPR, (void *) 5, "<", INT_LITERAL_EXPR, (void *) 5},
              {"5 == 5;", INT_LITERAL_EXPR, (void *) 5, "==", INT_LITERAL_EXPR, (void *) 5},
              {"5 != 5;", INT_LITERAL_EXPR, (void *) 5, "!=", INT_LITERAL_EXPR, (void *) 5},
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statements, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (statement->type != EXPR_STMT)
        {
            Fmt_print("not expression_statement got=%d\n", statement->type);
            goto cleanup;
        }
        expression_statement = (struct expression_statement *) statement;
        if (test_infix_expression(expression_statement->expression, tests[i].ltype,
                                  tests[i].lvalue, tests[i].operator,
                                  tests[i].rtype, tests[i].rvalue) != 0)
        {
            goto cleanup;
        }          
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;       
}

static int test_if_expression(void)
{
    const char *input = "if (x < y) { x }";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct if_expression *if_expression = NULL;
    char *str;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statements, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != IF_EXPR)
    {
        Fmt_print("not if got=%d\n", expression_statement->expression->type);
        return -1;
    }
    if_expression = (struct if_expression *) expression_statement->expression;
    if (test_infix_expression(if_expression->condition, IDENT_EXPR, "x", "<",
                              IDENT_EXPR, "y") != 0)
    {
        goto cleanup;
    }
    if (Seq_length(if_expression->consequence->statements) != 1)
    {
        Fmt_print("consequence does not contain 1 statements, got=%d\n",
                  Seq_length(if_expression->consequence->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(if_expression->consequence->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (test_identifier(expression_statement->expression, "x") != 0)
    {
        goto cleanup;
    }
    if (if_expression->alternative != NULL)
    {
        str = block_statement_to_string(if_expression->alternative);
        Fmt_print("alternative was not nil, got=%s\n", str);
        FREE(str);
        goto cleanup;
    }
    success = 0;
        
cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_if_else_expression(void)
{
    const char *input = "if (x < y) { x } else { y }";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct if_expression *if_expression = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statements, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != IF_EXPR)
    {
        Fmt_print("not if got=%d\n", expression_statement->expression->type);
        return -1;
    }
    if_expression = (struct if_expression *) expression_statement->expression;
    if (test_infix_expression(if_expression->condition, IDENT_EXPR, "x", "<",
                              IDENT_EXPR, "y") != 0)
    {
        goto cleanup;
    }
    if (Seq_length(if_expression->consequence->statements) != 1)
    {
        Fmt_print("consequence does not contain 1 statements, got=%d\n",
                  Seq_length(if_expression->consequence->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(if_expression->consequence->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (test_identifier(expression_statement->expression, "x") != 0)
    {
        goto cleanup;
    }
    if (if_expression->alternative == NULL)
    {
        Fmt_print("alternative was nil, got=%s\n");
        goto cleanup;
    }
    if (Seq_length(if_expression->alternative->statements) != 1)
    {
        Fmt_print("alternative does not contain 1 statements, got=%d\n",
                  Seq_length(if_expression->alternative->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(if_expression->alternative->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (test_identifier(expression_statement->expression, "y") != 0)
    {
        goto cleanup;
    }
    success = 0;
        
cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_operator_precedence_parsing()
{
    struct tests
    {
        const char *input;
        const char *expected;
    } tests[] =
          {
              /*{
                  "a + add(b * c) + d",
                  "((a + add((b * c))) + d)",
                  },*/
              {
                  "add(a, b, 1, 2 * 3, 4 + 5, add(6, 7 * 8))",
                  "add(a, b, 1, (2 * 3), (4 + 5), add(6, (7 * 8)))",
              },
              {
                  "add(a + b + c * d / f + g)",
                  "add((((a + b) + ((c * d) / f)) + g))",
              },
              {
                  "-a * b",
                  "((-a) * b)"
              },
              {
                  "!-a",
                  "(!(-a))"
              },
              {
                  "a + b + c",
                  "((a + b) + c)"
              },
              {
                  "a + b - c",
                  "((a + b) - c)"
              },
              {
                  "a * b * c",
                  "((a * b) * c)"
              },
              {
                  "a * b / c",
                  "((a * b) / c)"
              },
              {
                  "a + b / c",
                  "(a + (b / c))"
              },
              {
                  "a + b * c + d / e - f",
                  "(((a + (b * c)) + (d / e)) - f)"
              },
              {
                  "3 + 4; -5 * 5",
                  "(3 + 4)((-5) * 5)"
              },
              {
                  "5 > 4 == 3 < 4",
                  "((5 > 4) == (3 < 4))"
              },
              {
                  "5 < 4 != 3 > 4",
                  "((5 < 4) != (3 > 4))"
              },
              {
                  "3 + 4 * 5 == 3 * 1 + 4 * 5",
                  "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"
              },
              {
                  "3 + 4 * 5 == 3 * 1 + 4 * 5",
                  "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"
              },
              {
                  "1 + (2 + 3) + 4",
                  "((1 + (2 + 3)) + 4)"
              },
              {
                  "(5 + 5) * 2",
                  "((5 + 5) * 2)"
              },
              {
                  "2 / (5 + 5)",
                  "(2 / (5 + 5))"
              },
              {
                  "-(5 + 5)",
                  "(-(5 + 5))"
              },
              {
                  "!(true == true)",
                  "(!(true == true))"
              },
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    char *actual = NULL;
    int success = -1;


    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        actual = program_to_string(program);
        if (actual == NULL)
        {
            Fmt_print("program_to_string return NULL\n");
            goto cleanup;
        }
        if (Str_cmp(actual, 1, 0, tests[i].expected, 1, 0) != 0)
        {
            Fmt_print("expected=%s, got=%s\n", tests[i].expected, actual);
            goto cleanup;
        }
        FREE(actual);
        actual = NULL;
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (actual != NULL)
    {
        FREE(actual);
    }
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_function_literal_parsing(void)
{
    const char *input = "fn(x, y) { x + y; }";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct function_literal *function_literal = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statement, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != FUNC_LITERAL_EXPR)
    {
        Fmt_print("not function literal got=%d\n",
                  expression_statement->expression->type);
        goto cleanup;
    }
    function_literal = (struct function_literal *) expression_statement->expression;
    if (Seq_length(function_literal->parameters) != 2)
    {
        Fmt_print("function literal parameters wrong. want 2, got=%d\n",
                  Seq_length(function_literal->parameters));
        goto cleanup;
    }
    if (test_identifier((struct expression *) Seq_get(function_literal->parameters, 0), "x") != 0)
    {
        goto cleanup;
    }
    if (test_identifier((struct expression *) Seq_get(function_literal->parameters, 1), "y") != 0)
    {
        goto cleanup;
    }
    if (Seq_length(function_literal->body->statements) != 1)
    {
        Fmt_print("function body has not 1 statements got=%d\n",
                  Seq_length(function_literal->body->statements));
    }
    statement = (struct statement *) Seq_get(function_literal->body->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (test_infix_expression(expression_statement->expression, IDENT_EXPR, "x",
                              "+", IDENT_EXPR, "y") != 0)
    {
        goto cleanup;
    }
    success = 0;
        
cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    lexer_destroy(lexer);
    parser_destroy(parser);
    return success;
}

static int test_function_parameter_parsing()
{
    struct tests
    {
        const char *input;
        int len;
        const char *expected[10];
    } tests[] =
          {
              {
                  "fn() {};",
                  0,
                  { 0 }
              },
              {
                  "fn(x) {};",
                  1,
                  { "x" }
              },
              {
                  "fn(x, y, z) {};",
                  3,
                  { "x", "y", "z" }
              }
          };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct function_literal *function_literal = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        lexer = lexer_alloc(tests[i].input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (program == NULL)
        {
            Fmt_print("parser_parse_program return NULL\n");
            goto cleanup;
        }
        if (check_parse_errors(parser) != 0)
        {
            goto cleanup;
        }
        if (Seq_length(program->statements) != 1)
        {
            Fmt_print("program statements does not contain 1 statement, got=%d\n",
                      Seq_length(program->statements));
            goto cleanup;
        }
        statement = (struct statement *) Seq_get(program->statements, 0);
        if (statement->type != EXPR_STMT)
        {
            Fmt_print("not expression_statement got=%d\n", statement->type);
            goto cleanup;
        }
        expression_statement = (struct expression_statement *) statement;
        if (expression_statement->expression->type != FUNC_LITERAL_EXPR)
        {
            Fmt_print("not function literal got=%d\n",
                      expression_statement->expression->type);
            goto cleanup;
        }
        function_literal = (struct function_literal *) expression_statement->expression;
        if (Seq_length(function_literal->parameters) != tests[i].len)
        {
            Fmt_print("function literal parameters wrong. want %d, got=%d\n",
                      tests[i].len, Seq_length(function_literal->parameters));
            goto cleanup;
        }
        for (int j = 0; j < tests[i].len; j++)
        {
            if (test_identifier((struct expression *) Seq_get(function_literal->parameters, j),
                                tests[i].expected[j]) != 0)
            {
                goto cleanup;
            }            
        }
        program_destroy(program);
        program = NULL;
        lexer_destroy(lexer);
        lexer = NULL;
        parser_destroy(parser);
        parser = NULL;
    }
    success = 0;

cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

static int test_call_expression_parsing(void)
{
    const char *input = "add(1, 2 * 3, 4 + 5);";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    struct expression_statement *expression_statement = NULL;
    struct call_expression *call_expression = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("parser_parse_program return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 1)
    {
        Fmt_print("program statements does not contain 1 statement, got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    statement = (struct statement *) Seq_get(program->statements, 0);
    if (statement->type != EXPR_STMT)
    {
        Fmt_print("not expression_statement got=%d\n", statement->type);
        goto cleanup;
    }
    expression_statement = (struct expression_statement *) statement;
    if (expression_statement->expression->type != CALL_EXPR)
    {
        Fmt_print("not call expression got=%d\n",
                  expression_statement->expression->type);
        goto cleanup;
    }
    call_expression = (struct call_expression *) expression_statement->expression;
    if (test_identifier(call_expression->function, "add") != 0)
    {
        goto cleanup;
    }
    if (Seq_length(call_expression->arguments) != 3)
    {
        Fmt_print("wrong length of arguments, got=%d\n",
                  Seq_length(call_expression->arguments));
        goto cleanup;
    }
    if (test_literal_expression((struct expression *) Seq_get(call_expression->arguments, 0),
                                 INT_LITERAL_EXPR, (void *) 1) != 0)
    {
        goto cleanup;
    }
    if (test_infix_expression((struct expression *) Seq_get(call_expression->arguments, 1),
                              INT_LITERAL_EXPR, (void *) 2, "*", INT_LITERAL_EXPR, (void *) 3) != 0)
    {
        goto cleanup;
    }
    if (test_infix_expression((struct expression *) Seq_get(call_expression->arguments, 2),
                              INT_LITERAL_EXPR, (void *) 4, "+", INT_LITERAL_EXPR, (void *) 5) != 0)
    {
        goto cleanup;
    }
    success = 0;
        
cleanup:
    if (program != NULL)
    {
        program_destroy(program);
    }
    if (lexer != NULL)
    {
        lexer_destroy(lexer);
    }
    if (parser != NULL)
    {
        parser_destroy(parser);
    }
    return success;
}

int main(void)
{
    if (test_let_statements() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_return_statements() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_identifier_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_string_literal_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_integer_literal_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_boolean_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_prefix_expressions() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_infix_expressions() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_if_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_if_else_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_function_literal_parsing() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_function_parameter_parsing() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_call_expression_parsing() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_operator_precedence_parsing() != 0)
    {
        return EXIT_FAILURE;
    }
    printf("Tests successful\n");
    return EXIT_SUCCESS;
}
