#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <mem.h>
#include <str.h>
#include <seq.h>

#include "object.h"
#include "util.h"
#include "ast.h"

const char *object_type_str [] =
{
    [INTEGER_OBJ] = "INTEGER",
    [BOOLEAN_OBJ] = "BOOLEAN",
    [STRING_OBJ] = "STRING",
    [ARRAY_OBJ] = "ARRAY",
    [HASH_OBJ] = "HASH",
    [FUNC_OBJ] = "FUNC",
    [BUILTIN_OBJ] = "BUILTIN",
    [RETURN_VALUE_OBJ] = "RETURN VALUE",
    [ENV_OBJ] = "ENV",
    [NULL_OBJ] = "NULL",
    [ERROR_OBJ] = "ERROR"
};

static Seq_T allocated_objects;
struct boolean_object true_object = { BOOLEAN_OBJ, false, true, "true" };
struct boolean_object false_object  = { BOOLEAN_OBJ, false, false, "false" };
struct null_object null_object = { NULL_OBJ, false, "null" };
static void objects_mark(struct object *object);

int object_cmp(const void *x, const void *y)
{
    struct object *o1 = (struct object *) x;
    struct object *o2 = (struct object *) y;
    int diff;

    diff = o1->type - o2->type;
    if (diff != 0)
    {
        return diff;
    }
    switch (o1->type)
    {
    case INTEGER_OBJ:
    {
        return ((struct integer_object *) o1)->value -
            ((struct integer_object *) o2)->value;
    }
    case BOOLEAN_OBJ:
    {
        return ((struct boolean_object *) o1)->value -
            ((struct boolean_object *) o2)->value;
    }
    case STRING_OBJ:
    {
        return Str_cmp(((struct string_object *) o1)->value, 1, 0,
                       ((struct string_object *) o1)->value, 1, 0);
    }
    default:
    {
        break;
    }
    }
    return 0;
}

unsigned object_hash(const void *x)
{
    struct object *o = (struct object *) x;
    const char *str;
    unsigned h = 0;

    switch (o->type)
    {
    case INTEGER_OBJ:
    {
        return ((struct integer_object *) o)->value;
    }
    case BOOLEAN_OBJ:
    {
        return ((struct boolean_object *) o)->value;
    }
    case STRING_OBJ:
    {
        str = ((struct string_object *) o)->value;
        while (*str)
        {
            h = (h << 1) + *str++;
        }
        return h;       
    }
    default:
    {
        break;
    }
    }
    return 0;      
}

static Text_T *copy_text(Text_T t)
{
    Text_T *p;

    NEW0(p);
    *p = Text_box(Text_get(NULL, 0, t), t.len);
    return p;
}

static void free_text(Text_T *t)
{
    char *c = (char *) t->str;
    FREE(c);
    FREE(t);
}

static void free_store(const void *key, void **value, void *cl)
{
    free_text((Text_T *) key);
}

static void integer_object_destroy(struct integer_object *integer)
{
    FREE(integer);
}

static char *integer_object_inspect(struct integer_object *integer)
{
    return integer->inspect;
}

static char *boolean_object_inspect(struct boolean_object *boolean)
{
    return boolean->inspect;
}

static void string_object_destroy(struct string_object *string)
{
    FREE(string->value);
    FREE(string);
}

static char *string_object_inspect(struct string_object *string)
{
    return string->value;
}

static void array_object_destroy(struct array_object *array)
{
    Seq_free(&array->elements);
    FREE(array->inspect);
    FREE(array);
}

static char *array_object_inspect(struct array_object *array)
{
    return array->inspect;
}

static void hash_object_destroy(struct hash_object *hash)
{
    Table_free(&hash->pairs);
    FREE(hash->inspect);
    FREE(hash);
}

static char *hash_object_inspect(struct hash_object *hash)
{
    return hash->inspect;
}

static void function_object_destroy(struct function_object *function)
{
    function_literal_destroy(function->value);
    FREE(function->inspect);
    FREE(function);
}

static char *function_object_inspect(struct function_object *function)
{
    return function->inspect;
}

static char *builtin_object_inspect(struct builtin_object *builtin)
{
    return "builtin function";
}

static void return_value_destroy(struct return_value *return_value)
{
    FREE(return_value);
}

static char *return_value_inspect(struct return_value *return_value)
{
    return object_inspect(return_value->value);
}

static char *null_object_inspect(struct null_object *null)
{
    return null->inspect;
}

static void error_object_destroy(struct error_object *error_object)
{
    FREE(error_object);
}

static char *error_object_inspect(struct error_object *error)
{
    return error->value;
}

static void env_object_destroy(struct env_object *env)
{
    Table_map(env->store, free_store, NULL);
    Table_free(&env->store);
    FREE(env);
}

enum object_type object_type(struct object *object)
{
    return object->type;
}

void object_destroy(struct object *object)
{
    switch (object->type)
    {
    case INTEGER_OBJ:
    {
        integer_object_destroy((struct integer_object *) object);
        break;
    }
    case STRING_OBJ:
    {
        string_object_destroy((struct string_object *) object);
        break;
    }
    case ARRAY_OBJ:
    {
        array_object_destroy((struct array_object *) object);
        break;
    }
    case HASH_OBJ:
    {
        hash_object_destroy((struct hash_object *) object);
        break;
    }
    case FUNC_OBJ:
    {
        function_object_destroy((struct function_object *) object);
        break;
    }
    case RETURN_VALUE_OBJ:
    {
        return_value_destroy((struct return_value *) object);
        break;
    }
    case ENV_OBJ:
    {
        env_object_destroy((struct env_object *) object);
        break;
    }
    case ERROR_OBJ:
    {
        error_object_destroy((struct error_object *) object);
        break;
    }
    default:
    {
    }
    }
}

char *object_inspect(struct object *object)
{
    switch (object->type)
    {
    case INTEGER_OBJ:
    {
        return integer_object_inspect((struct integer_object *) object);
    }
    case BOOLEAN_OBJ:
    {
        return boolean_object_inspect((struct boolean_object *) object);
    }
    case STRING_OBJ:
    {
        return string_object_inspect((struct string_object *) object);
    }
    case ARRAY_OBJ:
    {
        return array_object_inspect((struct array_object *) object);
    }
    case HASH_OBJ:
    {
        return hash_object_inspect((struct hash_object *) object);
    }
    case FUNC_OBJ:
    {
        return function_object_inspect((struct function_object *) object);
    }
    case BUILTIN_OBJ:
    {
        return builtin_object_inspect((struct builtin_object *) object);
    }
    case RETURN_VALUE_OBJ:
    {
        return return_value_inspect((struct return_value *) object);
    }
    case NULL_OBJ:
    {
        return null_object_inspect((struct null_object *) object);
    }
    case ERROR_OBJ:
    {
        return error_object_inspect((struct error_object *) object);
    }
    default:
    {
        return NULL;
    }
    }
    return NULL;
}

struct integer_object *integer_object_alloc(long long value)
{
    struct integer_object *integer;
    
    NEW0(integer);
    integer->type = INTEGER_OBJ;
    integer->value = value;
    snprintf(integer->inspect, sizeof integer->inspect, "%lld", integer->value);
    Seq_addhi(allocated_objects, integer);
    return integer;
}

struct string_object *string_object_alloc(Text_T value)
{
    struct string_object *string;
    
    NEW0(string);
    string->type = STRING_OBJ;
    string->value = Text_get(NULL, 0, value);
    Seq_addhi(allocated_objects, string);
    return string;
}

struct array_object *array_object_alloc(Seq_T elements)
{
    struct array_object *array;
    struct object *object;
    char *str;
    char *str1;
    char *str2;
    
    NEW0(array);
    array->type = ARRAY_OBJ;
    array->elements = elements;
    str1 = Str_dup("[", 1, 0, 1);
    if (Seq_length(array->elements) > 0)
    {
        object = (struct object *) Seq_get(array->elements, 0);
        str2 = object_inspect(object);
        str = Str_cat(str1, 1, 0, str2, 1, 0);
        FREE(str1);
        str1 = str;
        for (int i = 1; i < Seq_length(array->elements); i++)
        {
            object = (struct object *) Seq_get(array->elements, i);
            str2 = object_inspect(object);
            str = Str_catv(str1, 1, 0, ", ", 1, 0, str2, 1, 0, NULL);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, "]", 1, 0);
    FREE(str1);
    array->inspect = str;
    Seq_addhi(allocated_objects, array);
    return array;
}

struct hash_object *hash_object_alloc(Table_T pairs)
{
    struct hash_object *hash;
    struct object *object;
    int i = 0;
    void **array;
    char *str;
    char *str1;
    char *str2;
    char *str3;

    NEW0(hash);
    hash->type = HASH_OBJ;
    hash->pairs = pairs;
    array = Table_toArray(hash->pairs, NULL);
    str1 = Str_dup("{", 1, 0, 1);
    if (array[i] != NULL)
    {
        object = (struct object *) array[i++];
        str2 = object_inspect(object);
        object = (struct object *) array[i++];
        str3 = object_inspect(object);
        str = Str_catv(str1, 1, 0, str2, 1, 0, ":", 1, 0, str3, 1, 0, NULL);
        FREE(str1);
        str1 = str;
        while (array[i] != NULL)
        {
            object = (struct object *) array[i++];
            str2 = object_inspect(object);
            object = (struct object *) array[i++];
            str3 = object_inspect(object);
            str = Str_catv(str1, 1, 0, ", ", 1, 0, str2, 1, 0, ":", 1, 0, str3, 1, 0, NULL);
            FREE(str1);
            str1 = str;
        }
    }
    FREE(array);
    str = Str_cat(str1, 1, 0, "}", 1, 0);
    FREE(str1);
    hash->inspect = str;
    Seq_addhi(allocated_objects, hash);
    return hash;
}

struct function_object *function_object_alloc(struct function_literal *value,
                                              struct env_object *env)
{
    struct function_object *function;
    struct identifier *identifier;
    char *str;
    char *str1;
    char *str2;

    NEW0(function);
    function->type = FUNC_OBJ;
    function_literal_addref(value);
    function->value = value;
    function->env = env;
    str = ALLOC(sizeof "fn(");
    Fmt_sfmt(str, sizeof "fn(", "fn(");
    str1 = str;
    if (Seq_length(function->value->parameters) > 0)
    {
        identifier = (struct identifier *) Seq_get(function->value->parameters, 0);
        str2 = identifier_to_string(identifier);
        str = Str_cat(str, 1, 0, str2, 1, 0);
        FREE(str2);
        FREE(str1);
        str1 = str;
        for (int i = 1; i < Seq_length(function->value->parameters); i++)
        {
            identifier = (struct identifier *) Seq_get(function->value->parameters, i);
            str2 = identifier_to_string(identifier);
            str = Str_catv(str, 1, 0, ", ", 1, 0, str2, 1, 0, NULL);
            FREE(str2);
            FREE(str1);
            str1 = str;
        }
    }
    str = Str_cat(str1, 1, 0, ") {\n", 1, 0);
    FREE(str1);
    str1 = str;
    str2 = block_statement_to_string(function->value->body);
    str = Str_cat(str1, 1, 0, str2, 1, 0);
    FREE(str2);
    FREE(str1);
    str1 = str;
    str = Str_cat(str1, 1, 0, "\n", 1, 0);
    FREE(str1);
    function->inspect = str;
    Seq_addhi(allocated_objects, function);
    return function;
}

struct return_value *return_value_alloc(struct object *value)
{
    struct return_value *return_value;
    
    NEW0(return_value);
    return_value->type = RETURN_VALUE_OBJ;
    return_value->value = value;
    Seq_addhi(allocated_objects, return_value);
    return return_value;
}

struct boolean_object *boolean_object_alloc(bool value)
{
    return value ? &true_object : &false_object;
}

struct error_object *error_object_alloc(const char *value, ...)
{
    struct error_object *error;
    va_list_box box;
    
    NEW0(error);
    error->type = ERROR_OBJ;
    va_start(box.ap, value);
    Fmt_vsfmt(error->value, sizeof error->value, value, &box);
    va_end(box.ap);
    Seq_addhi(allocated_objects, error);
    return error;
}

struct env_object *env_object_alloc(struct env_object *outer)
{
    struct env_object *env;

    NEW0(env);
    env->type = ENV_OBJ;
    env->store = Table_new(0, text_cmp, text_hash);
    env->outer = outer;
    Seq_addhi(allocated_objects, env);
    return env;
}

struct object *env_get(struct env_object *env, Text_T name)
{
    struct object *value;
    
    value = (struct object *) Table_get(env->store, &name);
    if (value == NULL && env->outer != NULL)
    {
        value = env_get(env->outer, name);
    }
    return value;
}

struct object *env_set(struct env_object *env, Text_T name, struct object *value)
{
    struct object *prev = NULL;

    if (Table_get(env->store, &name) != NULL)
    {
        prev = (struct object *) Table_put(env->store, &name, value);
    }
    else
    {
        Table_put(env->store, copy_text(name), value);
        
    }
    return prev;
}

void objects_init(void)
{
    allocated_objects = Seq_new(100);
}

static void array_object_mark(struct array_object *array)
{
    struct object *object;
    
    array->marked = true;
    for (int i = 1; i < Seq_length(array->elements); i++)
    {
        object = (struct object *) Seq_get(array->elements, i);
        objects_mark(object);
    }
}

static void mark_hash_pairs(const void *key, void **value, void *cl)
{
    objects_mark((struct object *) key);
    objects_mark((struct object *) *value);
}

static void hash_object_mark(struct hash_object *hash)
{
    hash->marked = true;
    Table_map(hash->pairs, mark_hash_pairs, NULL);
}

static void return_value_mark(struct return_value *return_value)
{
    return_value->marked = true;
    objects_mark(return_value->value);
}

static void function_object_mark(struct function_object *function)
{
    function->marked = true;
    objects_mark((struct object *) function->env);
}

static void mark_store(const void *key, void **value, void *cl)
{
    objects_mark((struct object *) *value);   
}

static void env_object_mark(struct env_object *env)
{
    env->marked = true;
    Table_map(env->store, mark_store, NULL);
    
}

static void objects_mark(struct object *object)
{
    if (object->marked)
    {
        return;
    }
    switch (object->type)
    {
    case INTEGER_OBJ:
    case STRING_OBJ:
    case ERROR_OBJ:
    {
        object->marked = true;
        break;
    }
    case ARRAY_OBJ:
    {
        array_object_mark((struct array_object *) object);
        break;
    }
    case HASH_OBJ:
    {
        hash_object_mark((struct hash_object *) object);
        break;
    }
    case FUNC_OBJ:
    {
        function_object_mark((struct function_object *) object);
        break;
    }
    case RETURN_VALUE_OBJ:
    {
        return_value_mark((struct return_value *) object);
        break;
    }
    case ENV_OBJ:
    {
        env_object_mark((struct env_object *) object);
        break;
    }
    default:
    {
    }
    }    
}

void objects_gc(struct env_object *env)
{
    int len;
    struct object *object;

    objects_mark((struct object *) env);
    len = Seq_length(allocated_objects);
    for (int i = 0; i < len; i++)
    {
        object = (struct object *) Seq_remlo(allocated_objects);
        if (object->marked)
        {
            object->marked = false;
            Seq_addhi(allocated_objects, object);
        }
        else
        {
            object_destroy(object);            
        }
    }
}

void objects_destroy(void)
{
    while (Seq_length(allocated_objects) > 0)
    {
        object_destroy((struct object *) Seq_remlo(allocated_objects));
    }
    Seq_free(&allocated_objects);
}
