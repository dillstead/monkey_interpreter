#include <stdlib.h>
#include <string.h>

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

static int test_null_object(struct object *object)
{
    if (object != (struct object *) &null_object)
    {
        Fmt_print("object is not NULL. got=%d (%+v)", object->type);
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
              {"-10", -10 },
              {"5 + 5 + 5 + 5 - 10", 10},
              {"2 * 2 * 2 * 2 * 2", 32},
              {"-50 + 100 + -50", 0},
              {"5 * 2 + 10", 20},
              {"5 + 2 * 10", 25},
              {"20 + 2 * -10", 0},
              {"50 / 2 * 2 + 10", 60},
              {"2 * (5 + 10)", 30},
              {"3 * 3 * 3 + 10", 37},
              {"3 * (3 * 3) + 10", 37},
              {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50}
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
              {"false", false },
              {"true == true", true},
              {"false == false", true},
              {"true == false", false},
              {"true != false", true},
              {"false != true", true},
              {"(1 < 2) == true", true},
              {"(1 < 2) == false", false},
              {"(1 > 2) == true", false},
              {"(1 > 2) == false", true}
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

static int test_if_else_expressions(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"if (true) { 10 }", 10},
              {"if (false) { 10 }", 0 /* NULL */},
              {"if (1) { 10 }", 10},
              {"if (1 < 2) { 10 }", 10},
              {"if (1 > 2) { 10 }", 0 /* NULL */},
              {"if (1 > 2) { 10 } else { 20 }", 20},
              {"if (1 < 2) { 10 } else { 20 }", 10}
          };
    struct object *object;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (object->type == INTEGER_OBJ)
        {
            if (test_integer_object(object, tests[i].expected) != 0)
            {
                goto cleanup;
            }            
        }
        else
        {
            if (test_null_object(object) != 0)
            {
                goto cleanup;
            }            
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

static int test_return_statements(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"return 10;", 10},
              {"return 10; 9;", 10},
              {"return 2 * 5; 9;", 10},
              {"9; return 2 * 5; 9;", 10},
              { "if (10 > 1) { if (10 > 1) { return 10; } return 1; }", 10}
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

static int test_error_handling(void)
{
    struct test
    {
        const char *input;
        const char *expected;
    } tests[] =
          {
              {
                "5 + true;",
                "type mismatch: INTEGER + BOOLEAN"
              },
              {
                  "5 + true; 5;",
                  "type mismatch: INTEGER + BOOLEAN"
              },
              {
                  "-true",
                  "unknown operator: -BOOLEAN"
              },
              {
                  "true + false;",
                  "unknown operator: BOOLEAN + BOOLEAN"
              },
              {
                  "5; true + false; 5",
                  "unknown operator: BOOLEAN + BOOLEAN"
              },
              {
                  "if (10 > 1) { true + false; }",
                  "unknown operator: BOOLEAN + BOOLEAN"
              },
              {
                  "if (10 > 1) { if (10 > 1) { return true + false; } return 1; }",
                  "unknown operator: BOOLEAN + BOOLEAN"
              }
          };
    struct object *object;
    struct error_object *error_object;
    int success = -1;

    lexer_init();
    parser_init();
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (object->type != ERROR_OBJ)
        {
            Fmt_print("object is not error got=%d\n", object->type);
        }
        error_object = (struct error_object *) object;
        if (strcmp(error_object->value, tests[i].expected) != 0)
        {
            Fmt_print("object has wrong value got=%s, want=%s\n",
                      error_object->value, tests[i].expected);
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
    if (test_if_else_expressions() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_return_statements() != 0)
    {
        return EXIT_FAILURE;
    }
    if (test_error_handling() != 0)
    {
        return EXIT_FAILURE;
    }
    printf("Tests successful\n");
    return EXIT_SUCCESS;
}
