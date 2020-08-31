#include <stdio.h>

#include "repl.h"
#include <mem.h>

#include "token.h"
#include "lexer.h"
#include "parser.h"

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
    char input[1024];

    Fmt_register('T', Text_fmt);
    lexer_init();
    parser_init();
    while (true)
    {
        struct lexer *lexer;
        struct parser *parser;
        struct program *program;
        char *output;

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
            parser_destroy(parser);
            lexer_destroy(lexer);
            continue;
        }
        if (Seq_length(program->statements) > 0)
        {
            output = program_to_string(program);
            Fmt_print("%s\n", output);
            FREE(output);            
        }
        parser_destroy(parser);
        lexer_destroy(lexer);
    }
}
