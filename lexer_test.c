#include <stdlib.h>
#include <stdio.h>

#include "lexer.h"

static int test_next_token(void)
{
    const char *input = "let five = 5;"
        "let ten = 10;"
        "let add = fn(x, y) {"
        "x + y;"
        "};"
        "let result = add(five, ten);"
        "!-/*5;"
        "5 < 10 > 5;"
        "if (5 < 10) {"
	"return true;"
        "} else {"
	"return false;"
        "}"
        "10 == 10;"
        "10 != 9;"
        "\"foobar\""
        "\"foo bar\""
        "[1, 2];"
        "{\"foo\": \"bar\"}";
    struct token tokens[] =
        {
            {LET, {sizeof "let" - 1, "let"}},
            {IDENT, {sizeof "five" - 1, "five"}},
            {ASSIGN, {sizeof "=" - 1, "="}},
            {INT, {sizeof "5" - 1, "5"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {LET, {sizeof "let" - 1, "let"}},
            {IDENT, {sizeof "ten" - 1, "ten"}},
            {ASSIGN, {sizeof "=" - 1, "="}},
            {INT, {sizeof "10" - 1, "10"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {LET, {sizeof "let" - 1, "let"}},
            {IDENT, {sizeof "add" - 1, "add"}},
            {ASSIGN, {sizeof "=" - 1, "="}},
            {FUNCTION, {sizeof "fn" - 1, "fn"}},
            {LPAREN, {sizeof "(" - 1, "("}},
            {IDENT, {sizeof "x" - 1, "x"}},
            {COMMA, {sizeof "," - 1, ","}},
            {IDENT, {sizeof "y" - 1, "y"}},
            {RPAREN, {sizeof ")" - 1, ")"}},
            {LBRACE, {sizeof "{" - 1, "{"}},
            {IDENT, {sizeof "x" - 1, "x"}},
            {PLUS, {sizeof "+" - 1, "+"}},
            {IDENT, {sizeof "y" - 1, "y"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {RBRACE, {sizeof "}" - 1, "}"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {LET, {sizeof "let" - 1, "let"}},
            {IDENT, {sizeof "result" - 1, "result"}},
            {ASSIGN, {sizeof "=" - 1, "="}},
            {IDENT, {sizeof "add" - 1, "add"}},
            {LPAREN, {sizeof "(" - 1, "("}},
            {IDENT, {sizeof "five" - 1, "five"}},
            {COMMA, {sizeof "," - 1, ","}},
            {IDENT, {sizeof "ten" - 1, "ten"}},
            {RPAREN, {sizeof ")" - 1, ")"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {BANG, {sizeof "!" - 1, "!"}},
            {MINUS, {sizeof "-" - 1, "-"}},
            {SLASH, {sizeof "/" - 1, "/"}},
            {ASTERISK, {sizeof "*" - 1, "*"}},
            {INT, {sizeof "5" - 1, "5"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {INT, {sizeof "5" - 1, "5"}},
            {LT, {sizeof "<" - 1, "<"}},
            {INT, {sizeof "10" - 1, "10"}},
            {GT, {sizeof ">" - 1, ">"}},
            {INT, {sizeof "5" - 1, "5"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {IF, {sizeof "if" - 1, "if"}},
            {LPAREN, {sizeof "(" - 1, "("}},
            {INT, {sizeof "5" - 1, "5"}},
            {LT, {sizeof "<" - 1, "<"}},
            {INT, {sizeof "10" - 1, "10"}},
            {RPAREN, {sizeof ")" - 1, ")"}},
            {LBRACE, {sizeof "{" - 1, "{"}},
            {RET, {sizeof "return" - 1, "return"}},
            {TRUE, {sizeof "true" - 1, "true"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {RBRACE, {sizeof "}" - 1, "}"}},
            {ELZE, {sizeof "else" - 1, "else"}},
            {LBRACE, {sizeof "{" - 1, "{"}},
            {RET, {sizeof "return" - 1, "return"}},
            {FALSE, {sizeof "false" - 1, "false"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {RBRACE, {sizeof "}" - 1, "}"}},
            {INT, {sizeof "10" - 1, "10"}},
            {EQ, {sizeof "==" - 1, "=="}},
            {INT, {sizeof "10" - 1, "10"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {INT, {sizeof "10" - 1, "10"}},
            {NOT_EQ, {sizeof "!=" - 1, "!="}},
            {INT, {sizeof "9" - 1, "9"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {STRING, {sizeof "foobar" - 1, "foobar"}},
            {STRING, {sizeof "foo bar" - 1, "foo bar"}},
            {LBRAKET, {sizeof "[" - 1, "["}},
            {INT, {sizeof "1" - 1, "1"}},
            {COMMA, {sizeof "," - 1, ","}},
            {INT, {sizeof "2" - 1, "2"}},
            {RBRAKET, {sizeof "]" - 1, "]"}},
            {SEMICOLON, {sizeof ";" - 1, ";"}},
            {LBRACE, {sizeof "{" - 1, "{"}},
            {STRING, {sizeof "foo" - 1, "foo"}},
            {COLON, {sizeof ":" - 1, ":"}},
            {STRING, {sizeof "bar" - 1, "bar"}},
            {RBRACE, {sizeof "}" - 1, "}"}},
            {END, {sizeof "", ""}},
        };
    struct lexer lexer;

    Fmt_register('T', Text_fmt);
    setup_lexer();
    lexer_init(&lexer, input);
    for (int i = 0; i < (sizeof tokens / sizeof tokens[0]); i++)
    {
        struct token token = lexer_next_token(&lexer);
        if (token.type != tokens[i].type)
        {
            Fmt_print("tests[%d] - token type wrong. expected=%d, got=%d\n", 
                      i, tokens[i].type, token.type);
            return -1;
        }
        if (Text_cmp(token.literal, tokens[i].literal) != 0)
        {
            Fmt_print("tests[%d] - token literal wrong. expected=%T, got=%T\n",
                      i, &tokens[i].literal, &token.literal);
            return -1;
        }
    }
    return 0;
}

int main(void)
{
    if (test_next_token() != 0)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
