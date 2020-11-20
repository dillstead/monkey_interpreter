#include <stdio.h>

#include "repl.h"
#include <mem.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "evaluator.h"
#include "env.h"

static void print_parse_errors(struct parser *parser)
{
    char *msg;
    
    for (int i = 0; i < Seq_length(parser->errors); i++)
    {
        msg = (char *) Seq_get(parser->errors, i);
        Fmt_print("\t%s\n", msg);
    }    
}

void repl_start(void)
{
    struct environment *env;
    char input[1024];

    Fmt_register('T', Text_fmt);
    lexer_init();
    parser_init();
    env = environment_alloc();
    while (true)
    {
        struct lexer *lexer;
        struct parser *parser;
        struct program *program;
        struct object *object;

        Fmt_print(">> ");
        if (fgets(input, sizeof input, stdin) == NULL)
        {
            break;
        }
        lexer = lexer_alloc(input);
        parser = parser_alloc(lexer);
        program = parser_parse_program(parser);
        if (Seq_length(parser->errors) != 0)
        {
            print_parse_errors(parser);
            program_destroy(program);
            parser_destroy(parser);
            lexer_destroy(lexer);
            continue;
        }
        if (Seq_length(program->statements) > 0)
        {
            object = eval((struct node *) program, env);
            Fmt_print("%s\n", object_inspect(object));
            object_destroy(object);
        }
        program_destroy(program);
        parser_destroy(parser);
        lexer_destroy(lexer);
    }
    environment_destroy(env);
}
