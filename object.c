#include <stdbool.h>
#include <stdio.h>
#include <mem.h>

#include "object.h"

struct boolean_object true_object = { BOOLEAN_OBJ, true, "true" };
struct boolean_object false_object  = { BOOLEAN_OBJ, false, "false" };
struct null_object null_object = { NULL_OBJ, "null" };

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
    case RETURN_VALUE:
    {
        return_value_destroy((struct return_value *) object);
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
    }
    return NULL;
}

struct integer_object *integer_object_alloc(long long value)
{
    struct integer_object *integer;
    
    NEW0(integer);
    integer->type = INTEGER_OBJ;
    integer->value = value;
    return integer;
}

struct return_value *return_value_alloc(struct object *value)
{
    struct return_value *return_value;
    
    NEW0(return_value);
    return_value->type = RETURN_VALUE;
    return_value->value = value;
    return return_value;
}

struct boolean_object *boolean_object_alloc(bool value)
{
    return value ? &true_object : &false_object;
}

struct null_object *null_object_alloc(void)
{
    return &null_object;
}
