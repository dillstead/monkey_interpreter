#include <text.h>
#include <string.h>
#include <stdbool.h>
#include <mem.h>
#include <table.h>
#include "lexer.h"
#include "token.h"
#include "util.h"

static Table_T keywords;

static bool is_letter(char ch)
{
    return ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == '_';
}

static bool is_digit(char ch)
{
    return '0' <= ch && ch <= '9';
}

static enum token_type lookup_ident(Text_T *ident)
{
    enum token_type type;

    type = (enum token_type) Table_get(keywords, ident);
    if (type != ILLEGAL)
    {
        return type;
    }
    return IDENT;
}

static void read_char(struct lexer *lexer)
{
    if (lexer->read_position >= lexer->input_length)
    {
        lexer->ch = '\0';
    }
    else
    {
        lexer->ch = lexer->input[lexer->read_position];
    }
    lexer->position = lexer->read_position;
    lexer->read_position++;
}

static char peek_char(struct lexer *lexer)
{
    if (lexer->read_position >= lexer->input_length)
    {
        return '\0';
    }
    else
    {
        return lexer->input[lexer->read_position];
    }
    lexer->position = lexer->read_position;
    lexer->read_position++;
}

static Text_T read_identifier(struct lexer *lexer)
{
    int position = lexer->position;
    
    while (is_letter(lexer->ch))
    {
        read_char(lexer);
    }

    return Text_box(lexer->input + position, lexer->position - position);
}

static Text_T read_number(struct lexer *lexer)
{
    int position = lexer->position;
    
    while (is_digit(lexer->ch))
    {
        read_char(lexer);
    }

    return Text_box(lexer->input + position, lexer->position - position);
}

static Text_T read_string(struct lexer *lexer)
{
    int position = lexer->position + 1;

    do
    {
        read_char(lexer);
    } while (lexer->ch != '"');

    return Text_box(lexer->input + position, lexer->position - position);
}

static void skip_whitespace(struct lexer *lexer)
{
    while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n'
           || lexer->ch == '\r')
    {
        read_char(lexer);
    }
}

void lexer_init(void)
{
    static struct token tokens[] =
        {
            {LET, {sizeof "let" - 1, "let"}},
            {FUNCTION, {sizeof "fn" - 1, "fn"}},
            {TRUE, {sizeof "true" - 1, "true"}},
            {FALSE, {sizeof "false" - 1, "false"}},
            {IF, {sizeof "if" - 1, "if"}},
            {ELZE, {sizeof "else" - 1, "else"}},
            {RET, {sizeof "return" - 1, "return"}}
        };
    
    keywords = Table_new(0, text_cmp, text_hash);
    for (int i = 0; i < (sizeof tokens / sizeof tokens[0]); i++)
    {
        Table_put(keywords, &tokens[i].literal, (void *) tokens[i].type);
    }
}

struct lexer *lexer_alloc(const char *input)
{
    struct lexer *lexer;

    NEW0(lexer);
    lexer->input_length = strlen(input);
    lexer->input = input;
    lexer->position = 0;
    lexer->read_position = 0;
    read_char(lexer);
    return lexer;
}

void lexer_destroy(struct lexer *lexer)
{
    FREE(lexer);
}

struct token lexer_next_token(struct lexer *lexer)
{
    struct token token;

    skip_whitespace(lexer);

    switch (lexer->ch)
    {
    case '=':
    {
        if (peek_char(lexer) == '=')
        {
            read_char(lexer);
            token.type = EQ;
            token.literal = Text_box("==", sizeof "==" - 1);
        }
        else
        {
            token.type = ASSIGN;
            token.literal = Text_box("=", sizeof "=" - 1);
        }
        break;
    }
    case '+':
    {
        token.type = PLUS;
        token.literal = Text_box("+", sizeof "+" - 1);
        break;
    }
    case '-':
    {
        token.type = MINUS;
        token.literal = Text_box("-", sizeof "-" - 1);
        break;
    }
    case '!':
    {
        if (peek_char(lexer) == '=')
        {
            read_char(lexer);
            token.type = NOT_EQ;
            token.literal = Text_box("!=", sizeof "!=" - 1);
        }
        else
        {
            token.type = BANG;
            token.literal = Text_box("!", sizeof "!" - 1);
        }
        break;
    }
    case '/':
    {
        token.type = SLASH;
        token.literal = Text_box("/", sizeof "/" - 1);
        break;
    }
    case '*':
    {
        token.type = ASTERISK;
        token.literal = Text_box("*", sizeof "*" - 1);
        break;
    }
    case '<':
    {
        token.type = LT;
        token.literal = Text_box("<", sizeof "<" - 1);
        break;
    }
    case '>':
    {
        token.type = GT;
        token.literal = Text_box(">", sizeof ">" - 1);
        break;
    }
    case ';':
    {
        token.type = SEMICOLON;
        token.literal = Text_box(";", sizeof ";" - 1);
        break;
    }
    case '(':
    {
        token.type = LPAREN;
        token.literal = Text_box("(", sizeof "(" - 1);
        break;
    }
    case ')':
    {
        token.type = RPAREN;
        token.literal = Text_box(")", sizeof ")" - 1);
        break;
    }
    case ',':
    {
        token.type = COMMA;
        token.literal = Text_box(",", sizeof "," - 1);
        break;
    }
    case '{':
    {
        token.type = LBRACE;
        token.literal = Text_box("{", sizeof "{" - 1);
        break;
    }
    case '}':
    {
        token.type = RBRACE;
        token.literal = Text_box("}", sizeof "}" - 1);
        break;
    }
    case '[':
    {
        token.type = LBRAKET;
        token.literal = Text_box("[", sizeof "[" - 1);
        break;
    }
    case ']':
    {
        token.type = RBRAKET;
        token.literal = Text_box("]", sizeof "]"- 1);
        break;
    }
    case ':':
    {
        token.type = COLON;
        token.literal = Text_box(":", sizeof ":" - 1);
        break;
    }
    case '"':
    {
        token.type = STRING;
        token.literal = read_string(lexer);
        break;
    }
    case '\0':
    {
        token.type = END;
        token.literal = Text_box("", sizeof "");
        break;
    }
    default:
    {
        if (is_letter(lexer->ch))
        {
            token.literal = read_identifier(lexer);
            token.type = lookup_ident(&token.literal);
            return token;
        }
        else if (is_digit(lexer->ch))
        {
            token.type = INT;
            token.literal = read_number(lexer);
            return token;
        } 
        else
        {
            token.type = ILLEGAL;
            token.literal = Text_box(lexer->input + lexer->position, 1);
        }
    }
    }
    read_char(lexer);

    return token;
}

