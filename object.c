#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <mem.h>

#include "object.h"

const char *object_type_str [] =
{
    [INTEGER_OBJ] = "INTEGER",
    [BOOLEAN_OBJ] = "BOOLEAN",
    [NULL_OBJ] = "NULL"
};
    
struct boolean_object true_object = { BOOLEAN_OBJ, 1, true, "true" };
struct boolean_object false_object  = { BOOLEAN_OBJ, 1, false, "false" };
struct null_object null_object = { NULL_OBJ, 1, "null" };

static void integer_object_destroy(struct integer_object *integer)
{
    FREE(integer);
}

static char *integer_object_inspect(struct integer_object *integer)
{
    snprintf(integer->inspect, sizeof integer->inspect, "%lld", integer->value);
    return integer->inspect;
}

static char *boolean_object_inspect(struct boolean_object *boolean)
{
    return boolean->inspect;
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

enum object_type object_type(struct object *object)
{
    return object->type;
}

void object_addref(struct object *object)
{
    object->cnt++;
}

void object_delref(struct object *object)
{
    object_destroy(object);
}

void object_destroy(struct object *object)
{
    if (--object->cnt > 0)
    {
        return;
    }
    switch (object->type)
    {
    case INTEGER_OBJ:
    {
        integer_object_destroy((struct integer_object *) object);
        break;
    }
    case RETURN_VALUE:
    {
        return_value_destroy((struct return_value *) object);
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
    case RETURN_VALUE:
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
    }
    return NULL;
}

struct integer_object *integer_object_alloc(long long value)
{
    struct integer_object *integer;
    
    NEW0(integer);
    integer->type = INTEGER_OBJ;
    integer->cnt = 1;
    integer->value = value;
    return integer;
}

struct return_value *return_value_alloc(struct object *value)
{
    struct return_value *return_value;
    
    NEW0(return_value);
    return_value->type = RETURN_VALUE;
    return_value->cnt = 1;
    return_value->value = value;
    return return_value;
}

struct boolean_object *boolean_object_alloc(bool value)
{
    if (value)
    {
        true_object.cnt++;
        return &true_object;
    }
    else
    {
        false_object.cnt++;
        return &false_object;
    }
}

struct null_object *null_object_alloc(void)
{
    null_object.cnt++;
    return &null_object;
}

struct error_object *error_object_alloc(const char *value, ...)
{
    struct error_object *error;
    va_list_box box;
    
    NEW0(error);
    error->type = ERROR_OBJ;
    error->cnt = 1;
    va_start(box.ap, value);
    Fmt_vsfmt(error->value, sizeof error->value, value, &box);
    va_end(box.ap);
    return error;
}
