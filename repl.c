#include <stdio.h>

#include "repl.h"
#include "lexer.h"

void repl_start(void)
{
    char line[1024];

    Fmt_register('T', Text_fmt);
    setup_lexer();
    Fmt_print(">> ");
    while (fgets(line, sizeof line, stdin) != NULL)
    {
        struct lexer lexer;

        lexer_init(&lexer, line);

         for (struct token token = lexer_next_token(&lexer);
             token.type != END; token = lexer_next_token(&lexer))
        {
            Fmt_print("%T\n", &token.literal);
        }
        Fmt_print(">> ");
    }
}
