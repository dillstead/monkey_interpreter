#include <stdlib.h>
#include <string.h>
#include <mem.h>
#include <str.h>

#include "parser.h"
#include "evaluator.h"
#include "object.h"
#include "builtins.h"

static struct env_object *env;

static struct object *test_eval(const char *input)
{
    struct object *object;
    struct lexer *lexer = NULL;
    struct parser *parser = NULL;
    struct program *program = NULL;

    lexer = lexer_alloc(input);
    parser = parser_alloc(lexer);
    program = parser_parse_program(parser);
    object = eval((struct node *) program, env);
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
        Fmt_print("object is not integer got=%s\n", object_type_str[object->type]);
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
        Fmt_print("object is not boolean got=%s\n", object_type_str[object->type]);
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
        Fmt_print("object is not NULL. got=%d (%+v)", object_type_str[object->type]);
        return -1;
    }
    return 0;
}

static int test_eval_integer_expressions(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"5", 5},
              {"10", 10},
              {"-5", -5},
              {"-10", -10},
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

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_integer_object(object, tests[i].expected) != 0)
        {
            return -1;
        }
    }
    return 0;
}

static int test_eval_boolean_expressions(void)
{
    struct test
    {
        const char *input;
        bool expected;
    } tests[] =
          {
              {"true", true},
              {"false", false},
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

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_boolean_object(object, tests[i].expected) != 0)
        {
            return -1;
        }
    }
    return 0;
}

static int test_eval_string_expressions(void)
{
    const char *input = "\"Hello World!\"";
    char *expected = "Hello World!";
    struct object *object;
    struct string_object *string_object;

    object = test_eval(input);
    if (object->type != STRING_OBJ)
    {
        Fmt_print("object is not string got=%s\n", object_type_str[object->type]);
        return -1;
    }
    string_object = (struct string_object *) object;
    if (Str_cmp(string_object->value, 1, 0, expected, 1, 0) != 0)
    {
        Fmt_print("object has wrong value got=%s, want=%s\n",
                  string_object->value, expected);
        return -1;
    }
    return 0;
}

static int test_string_concatentation(void)
{
    const char *input = "\"Hello\" + \" \" + \"World!\"";
    char *expected = "Hello World!";
    struct object *object;
    struct string_object *string_object;

    object = test_eval(input);
    if (object->type != STRING_OBJ)
    {
        Fmt_print("object is not string got=%s\n", object_type_str[object->type]);
        return -1;
    }
    string_object = (struct string_object *) object;
    if (Str_cmp(string_object->value, 1, 0, expected, 1, 0) != 0)
    {
        Fmt_print("object has wrong value got=%s, want=%s\n",
                  string_object->value, expected);
        return -1;
    }
    return 0;
}

static int test_eval_array_expressions(void)
{
    const char *input = "[1, 2 * 2, 3 + 3]";
    struct object *object;
    struct object *element;
    struct array_object *array_object;

    object = test_eval(input);
    if (object->type != ARRAY_OBJ)
    {
        Fmt_print("object is not array got=%s\n", object_type_str[object->type]);
        return -1;
    }
    array_object = (struct array_object *) object;
    if (Seq_length(array_object->elements) != 3)
    {
        Fmt_print("array has wrong number of elements, got=%d\n",
                  Seq_length(array_object->elements));
        return -1;        
    }
    element = (struct object *) Seq_get(array_object->elements, 0);
    if (test_integer_object(element, 1) != 0)
    {
        return -1;
    }
    element = (struct object *) Seq_get(array_object->elements, 1);
    if (test_integer_object(element, 4) != 0)
    {
        return -1;
    }
    element = (struct object *) Seq_get(array_object->elements, 2);
    if (test_integer_object(element, 6) != 0)
    {
        return -1;
    }
    return 0;
}

static int test_eval_hash_expressions(void)
{
    const char *input = "let two = \"two\";"
        "{ \"one\": 10 - 9, two: 1 + 1, \"thr\" + \"ee\": 6 / 2, 4: 4, true: 5, false: 6 }";
    struct test
    {
        struct object *key;
        long long value;
    } expected[] =
          {
              {NULL, 1},
              {NULL, 2},
              {NULL, 3},
              {NULL, 4},
              {NULL, 5},
              {NULL, 6},
          };    
    struct object *object;
    struct object *value;
    struct hash_object *hash_object;

    expected[0].key = (struct object *) string_object_alloc((Text_T) {sizeof "one" - 1, "one"});
    expected[1].key = (struct object *) string_object_alloc((Text_T) {sizeof "two" - 1, "two"});
    expected[2].key = (struct object *) string_object_alloc((Text_T) {sizeof "three" - 1, "three"});
    expected[3].key = (struct object *) integer_object_alloc(4);
    expected[4].key = (struct object *) &true_object;
    expected[5].key = (struct object *) &false_object;
    object = test_eval(input);
    if (object->type != HASH_OBJ)
    {
        Fmt_print("object is not hash got=%s\n", object_type_str[object->type]);
        return -1;
    }
    hash_object = (struct hash_object *) object;
    if (Table_length(hash_object->pairs) != 6)
    {
        Fmt_print("hash has wrong number of elements, got=%d\n",
                  Table_length(hash_object->pairs));
        return -1;        
    }
    for (int i = 0; i < sizeof expected / sizeof expected[0]; i++)
    {
        value = (struct object *) Table_get(hash_object->pairs, expected[i].key);
        if (value == NULL)
        {
            Fmt_print("no pair for given key\n");
            return -1;
        }
        if (test_integer_object(value, expected[i].value) != 0)
        {
            return -1;
        }            
    }
    return 0;
}

static int test_hash_index_expressions(void)
{
    struct test
    {
        const char *input;
        enum object_type type;
        long long expected;
    } tests[] =
          {
              {"{\"foo\": 5}[\"foo\"]", INTEGER_OBJ, 5},
              {"{\"foo\": 5}[\"bar\"]", NULL_OBJ},
              {"let key = \"foo\"; {\"foo\": 5}[key]", INTEGER_OBJ, 5},
              {"{}[\"foo\"]", NULL_OBJ},
              {"{5: 5}[5]", INTEGER_OBJ, 5},
              {"{true: 5}[true]", INTEGER_OBJ, 5},
              {"{false: 5}[false]", INTEGER_OBJ, 5}
          };
    struct object *object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (tests[i].type == INTEGER_OBJ)
        {
            if (test_integer_object(object, tests[i].expected) != 0)
            {
                return -1;
            }            
        }
        else if (tests[i].type == NULL_OBJ)
        {
            if (test_null_object(object) != 0)
            {
                return -1;
            }
        }
    }
    return 0;
}

static int test_array_index_expressions(void)
{
    struct test
    {
        const char *input;
        enum object_type type;
        long long expected;
    } tests[] =
          {
              {"[1, 2, 3][0]", INTEGER_OBJ, 1},
              {"[1, 2, 3][1]", INTEGER_OBJ, 2},
              {"[1, 2, 3][2]", INTEGER_OBJ, 3},
              {"let i = 0; [1][i];", INTEGER_OBJ, 1},
              {"[1, 2, 3][1 + 1];", INTEGER_OBJ, 3},
              {"let myArray = [1, 2, 3]; myArray[2];", INTEGER_OBJ, 3},
              {"let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];", INTEGER_OBJ, 6},
              {"let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]", INTEGER_OBJ, 2},
              {"[1, 2, 3][3]", NULL_OBJ},
              {"[1, 2, 3][-1]", NULL_OBJ}
          };
    struct object *object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (tests[i].type == INTEGER_OBJ)
        {
            if (test_integer_object(object, tests[i].expected) != 0)
            {
                return -1;
            }            
        }
        else if (tests[i].type == NULL_OBJ)
        {
            if (test_null_object(object) != 0)
            {
                return -1;
            }
        }
    }
    return 0;
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

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_boolean_object(object, tests[i].expected) != 0)
        {
            return -1;
        }
    }
    return 0;
}

static int test_if_else_expressions(void)
{
    struct test
    {
        const char *input;
        enum object_type type;
        long long expected;
    } tests[] =
          {
              {"if (true) { 10 }", INTEGER_OBJ, 10},
              {"if (false) { 10 }", NULL_OBJ},
              {"if (1) { 10 }", INTEGER_OBJ, 10},
              {"if (1 < 2) { 10 }", INTEGER_OBJ, 10},
              {"if (1 > 2) { 10 }", NULL_OBJ},
              {"if (1 > 2) { 10 } else { 20 }", INTEGER_OBJ, 20},
              {"if (1 < 2) { 10 } else { 20 }", INTEGER_OBJ, 10}
          };
    struct object *object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (tests[i].type == INTEGER_OBJ)
        {
            if (test_integer_object(object, tests[i].expected) != 0)
            {
                return -1;
            }            
        }
        else if (tests[i].type == NULL_OBJ)
        {
            if (test_null_object(object) != 0)
            {
                return -1;
            }
        }
    }
    return 0;
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

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_integer_object(object, tests[i].expected) != 0)
        {
            return -1;
        }            
    }
    return 0;
}

static int test_error_handling(void)
{
    struct test
    {
        const char *input;
        const char *expected;
    } tests[] =
          {
              /*{
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
              },
              {
                  "foobar",
                  "identifier not found: foobar"
              },
              {
                  "\"Hello\" - \"World\"",
                  "unknown operator: STRING - STRING"
              },
              {
                  "len(1)",
                  "argument to 'len' not supported, got INTEGER"
              },
              {
                  "len(\"one\", \"two\")",
                  "wrong number of arguments. got=2, want=1"
              },
              {
                  "first(1)",
                  "argument to 'first' must be ARRAY, got INTEGER"
              },
              {
                  "last(1)",
                  "argument to 'last' must be ARRAY, got INTEGER"
              },
              {    "rest(1)",
                   "argument to 'rest' must be ARRAY, got INTEGER"
              },
              {
                  "push([])",
                  "wrong number of arguments. got=1, want=2"
              },
	      {
                  "push(1, 2)",
                  "first argument to 'push' must be ARRAY, got INTEGER"
              },*/
              {
                  "{ []: 1 + 1 }",
                  "unusable as hash key, got ARRAY"
              },
              /*{
                  "{\"name\": \"Monkey\"}[fn(x) { x }];",
                  "unusable as hash key, got FUNC",
              },
              {
                  "\"name\"[0]",
                  "index operator not supported: STRING"
              }*/
          };
    struct object *object;
    struct error_object *error_object;
 
    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (object->type != ERROR_OBJ)
        {
            Fmt_print("object is not error got=%s\n", object_type_str[object->type]);
            return -1;
        }
        error_object = (struct error_object *) object;
        if (strcmp(error_object->value, tests[i].expected) != 0)
        {
            Fmt_print("object has wrong value got=%s, want=%s\n",
                      error_object->value, tests[i].expected);
            return -1;
        }            
    }
    return 0;
}

static int test_let_statements(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"let a = 5; a;", 5},
              {"let a = 5 * 5; a;", 25},
              {"let a = 5; let b = a; b;", 5},
              {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
          };
    struct object *object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_integer_object(object, tests[i].expected) != 0)
        {
            return -1;
        }
    }
    return 0;
}

static int test_function_object(void)
{
    const char *input = "fn(x) { x + 2; };";
    const char *expected_body = "(x + 2)";
    struct object *object;
    struct function_object *function_object;
    char *str = NULL;
    int success = -1;
    
    object = test_eval(input);
    if (object->type != FUNC_OBJ)
    {
        Fmt_print("object is not function got=%s\n", object_type_str[object->type]);
        goto cleanup;
    }
    function_object = (struct function_object *) object;
    if (Seq_length(function_object->value->parameters) != 1)
    {
        Fmt_print("function has wrong parameters. want 1 got=%d\n",
                  Seq_length(function_object->value->parameters));
        goto cleanup;
    }
    str = identifier_to_string((struct identifier *) Seq_get(function_object->value->parameters, 0));
    if (strcmp(str, "x") != 0)
    {
        Fmt_print("parameter is not 'x'. got %s\n", str);
        goto cleanup;
    }
    FREE(str);
    str = block_statement_to_string(function_object->value->body);
    if (strcmp(str, expected_body) != 0)
    {
        Fmt_print("body is not %s. got %s\n", expected_body, str);
        goto cleanup;
    }
    FREE(str);
    success = 0;
       
cleanup:
    if (str != NULL)
    {
        FREE(str);
    }
    return success;
}

static int test_function_application(void)
{
    struct test
    {
        const char *input;
        long long expected;
    } tests[] =
          {
              {"let identity = fn(x) { x; }; identity(5);", 5},
              {"let identity = fn(x) { return x; }; identity(5);", 5},
              {"let double = fn(x) { x * 2; }; double(5);", 10},
              {"let add = fn(x, y) { x + y; }; add(5, 5);", 10},
              {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
              {"fn(x) { x; }(5)", 5},
          };
    struct object *object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (test_integer_object(object, tests[i].expected) != 0)
        {
            return -1;
        }
    }
    return 0;
}

static int test_builtin_functions(void)
{
    struct test
    {
        const char *input;
        enum object_type type;
        long long value;
        long long elements[5];
    } tests[] =
          {
              {"len(\"\")", INTEGER_OBJ, 0},
              {"len(\"four\")", INTEGER_OBJ, 4},
              {"len(\"hello world\")", INTEGER_OBJ, 11},
              {"len([])", INTEGER_OBJ, 0},
              {"len([1])", INTEGER_OBJ, 1},
              {"len([1, 1 + 2 * 3, true])", INTEGER_OBJ, 3},
              {"first([])", NULL_OBJ},
              {"first([1])", INTEGER_OBJ, 1},
              {"first([1, 2])", INTEGER_OBJ, 1},
              {"last([])", NULL_OBJ},
              {"last([1])", INTEGER_OBJ, 1},
              {"last([1, 2])", INTEGER_OBJ, 2},
              {"rest([])", NULL_OBJ},
              {"rest([1])", ARRAY_OBJ, 0},
              {"rest([1, 2, 3])", ARRAY_OBJ, 2, {2, 3}},
              {"push([], 1)", ARRAY_OBJ, 1, {1}},
              {"push([1, 2], 3)", ARRAY_OBJ, 3, {1, 2, 3}},
           };
    struct object *object;
    struct array_object *array_object;

    for (int i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        object = test_eval(tests[i].input);
        if (tests[i].type == INTEGER_OBJ)
        {
            if (test_integer_object(object, tests[i].value) != 0)
            {
                return -1;
            }            
        }
        else if (tests[i].type == ARRAY_OBJ)
        {
            if (object->type != ARRAY_OBJ)
            {
                Fmt_print("object is not array got=%s\n", object_type_str[object->type]);
                return -1;
            }
            array_object = (struct array_object *) object;
            if (Seq_length(array_object->elements) != tests[i].value)
            {
                Fmt_print("wrong number of elements. want=%d, got=%d", tests[i].value,
                          Seq_length(array_object->elements));
                return -1;
            }
            for (int j = 0; j < tests[i].elements[j]; j++)
            {
                if (test_integer_object(Seq_get(array_object->elements, j), tests[i].elements[j]) != 0)
                {
                    return -1;
                }
            }
        }
        else if (tests[i].type == NULL_OBJ)
        {
            if (test_null_object(object) != 0)
            {
                return -1;
            }
        }
    }
    return 0;
}

int main(void)
{
    lexer_init();
    parser_init();
    builtins_init();
    objects_init();
    env = env_object_alloc(NULL);
    int rc = EXIT_SUCCESS;

    if (test_eval_integer_expressions() != 0)
    {
        printf("test_eval_integer_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_eval_boolean_expressions() != 0)
    {
        printf("test_eval_boolean_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_eval_string_expressions() != 0)
    {
        printf("test_eval_string_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_string_concatentation() != 0)
    {
        printf("test_eval_string_concatenation failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_eval_array_expressions() != 0)
    {
        printf("test_eval_array_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_array_index_expressions() != 0)
    {
        printf("test_array_index_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_eval_hash_expressions() != 0)
    {
        printf("test_eval_hash_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_hash_index_expressions() != 0)
    {
        printf("test_hash_index_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_bang_operator() != 0)
    {
        printf("test_bang_operator failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_if_else_expressions() != 0)
    {
        printf("test_if_else_expressions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_return_statements() != 0)
    {
        printf("test_return_statements failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_error_handling() != 0)
    {
        printf("test_error_handling failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_let_statements() != 0)
    {
        printf("test_let_statements failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_function_object() != 0)
    {
        printf("test_function_object failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_function_application() != 0)
    {
        printf("test_function_application failed\n");
        goto cleanup;
    }
    objects_gc(env);
    if (test_builtin_functions() != 0)
    {
        printf("test_builtin_functions failed\n");
        goto cleanup;
    }
    objects_gc(env);
    printf("Tests successful\n");
    rc = EXIT_SUCCESS;

cleanup:
    objects_destroy();
    return rc;
}
