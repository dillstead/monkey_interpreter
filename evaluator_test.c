#include <stdlib.h>

#include "parser.h"
#include "evaluator.h"

static struct object *test_eval(const char *input)
{
    struct object *object;
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;

    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    object = eval((struct node *) program);
    program_destroy(program);
    lexer_destroy(lexer);
    parser_destroy(parser);
    return object;
}

static int test_integer_object(struct object *object, long long expected)
{
    struct integer_object *integer_object;
    
    if (object->type != INTEGER_OBJ)
    {
        Fmt_print("object is not integer got=%d\n", object->type);
        return -1;
    }
    integer_object = (struct integer_object *) object;
    if (integer_object->value != expected)
    {
        Fmt_print("object has wrong value got=%d, want=%d\n",
                  integer_object->value, expected);
        return -1;
    }
    return 0;
}

static int test_boolean_object(struct object *object, bool expected)
{
    struct boolean_object *boolean_object;
    
    if (object->type != BOOLEAN_OBJ)
    {
        Fmt_print("object is not boolean got=%d\n", object->type);
        return -1;
    }
    boolean_object = (struct boolean_object *) object;
    if (boolean_object->value != expected)
    {
        Fmt_print("object has wrong value got=%d, want=%d\n",
                  boolean_object->value, expected);
        return -1;
    }
    return 0;
}

static int test_eval_integer_expression(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"5", 5 },
              {"10", 10 },
              {"-5", -5 },
              {"-10", -10 }
          };
    struct object *object;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_integer_object(object, tests[i].expected) != 0)
        {
            goto cleanup;
        }
        object_destroy(object);
        object = NULL;
    }
    success = 0;

cleanup:
    if (object != NULL)
    {
        object_destroy(object);
    }
    return success;
}

static int test_eval_boolean_expression(void)
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
    struct object *object;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_boolean_object(object, tests[i].expected) != 0)
        {
            goto cleanup;
        }
        object_destroy(object);
        object = NULL;
    }
    success = 0;

cleanup:
    if (object != NULL)
    {
        object_destroy(object);
    }
    return success;
}

static int test_bang_operator(void)
{
    struct test
    {
        const char *input;
        bool expected;
    } tests[] =
          {
              {"!true", false},
              {"!false", true},
              {"!5", false},
              {"!!true", true},
              {"!!false", false},
              {"!!5", true}
          };
    struct object *object;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_boolean_object(object, tests[i].expected) != 0)
        {
            goto cleanup;
        }
        object_destroy(object);
        object = NULL;
    }
    success = 0;

cleanup:
    if (object != NULL)
    {
        object_destroy(object);
    }
    return success;
}

int main(void)
{
    if (test_eval_integer_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_eval_boolean_expression() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_bang_operator() != 0)
    {
        return EXIT_FAILURE;
    }
    printf("Tests successful\n");
    return EXIT_SUCCESS;
}
