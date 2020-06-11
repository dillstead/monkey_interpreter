#include <stdlib.h>
#include <stdio.h>

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

static int test_let_statement(struct statement *s, Text_T name)
{
    struct let_statement *let_statement;
    Text_T let = { sizeof "let" - 1, "let"};

    if (s->type != LET_STMT)
    {
        Fmt_print("s not *ast.LetStatement. got=%d", s->type);
        return -1;
    }
    if (Text_cmp(statement_token_literal(s), let) != 0)

    {
        Fmt_print("s.TokenLiteral not 'let'. got=%T",
                  statement_token_literal(s));
        return -1;
    }
    let_statement = (struct let_statement *) s;
    if (Text_cmp(let_statement->name->value, name) != 0)
    {
        Fmt_print("letStmt.Name.Value not '%T'. got=%T", name,
                  let_statement->name->value);
        return -1;
    }
    if (Text_cmp(identifier_token_literal(let_statement->name), name) != 0)
    {
        Fmt_print("s.Name not '%T'. got=%s", name,
                  identifier_token_literal(let_statement->name));
        return -1;
    }
    return 0;
}

static int test_let_statements(void)
{
    const char *input = "let x = 5;"
        "let y = 10;"
        "let foobar = 838383;";
    struct Text_T tests[] =
        {
            {sizeof "x" - 1, "x"},
            {sizeof "y" - 1, "y"},
            {sizeof "foobar" - 1, "foobar"}
        };
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("ParseProgram() return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 3)
    {
        Fmt_print("program statements does not contain 3 statements. got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    for (int i = 0; i < Seq_length(program->statements); i++)
    {
        statement = (struct statement *) Seq_get(program->statements, i);
        if (test_let_statement(statement, tests[i]) != 0)
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


static int test_return_statements(void)
{
    Text_T ret = { sizeof "return" - 1, "return"};
    const char *input = "return 5;"
        "return 10;"
        "return 993322;";
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;
    struct statement *statement = NULL;
    int success = -1;

    lexer_init();
    parser_init();
    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    if (program == NULL)
    {
        Fmt_print("ParseProgram() return NULL\n");
        goto cleanup;
    }
    if (check_parse_errors(parser) != 0)
    {
        goto cleanup;
    }
    if (Seq_length(program->statements) != 3)
    {
        Fmt_print("program statements does not contain 3 statements. got=%d\n",
                  Seq_length(program->statements));
        goto cleanup;
    }
    for (int i = 0; i < Seq_length(program->statements); i++)
    {
        statement = (struct statement *) Seq_get(program->statements, i);
        if (statement->type != RETURN_STMT)
        {
            Fmt_print("s not *ast.ReturnStatemente. got=%d", statement->type);
            goto cleanup;
        }
        if (Text_cmp(statement_token_literal(statement), ret) != 0)
        {
            Fmt_print("s.TokenLiteral not 'ret'. got=%T",
                      statement_token_literal(statement));
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
    return EXIT_SUCCESS;
}
