#include <stdio.h>

#include "repl.h"
#include "token.h"
#include "lexer.h"

void repl_start(void)
{
    char line[1024];

    Fmt_register('T', Text_fmt);
    lexer_init();
    Fmt_print(">> ");
    while (fgets(line, sizeof line, stdin) != NULL)
    {
        struct lexer *lexer;

        lexer = lexer_alloc(line);
        for (struct token token = lexer_next_token(lexer);
             token.type != END; token = lexer_next_token(lexer))
        {
            Fmt_print("%T\n", &token.literal);
        }
        Fmt_print(">> ");
        lexer_destroy(lexer);
    }
}
