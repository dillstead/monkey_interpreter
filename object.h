#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>

enum object_type
{
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    RETURN_VALUE,
    NULL_OBJ
};

struct object
{
    enum object_type type;
};

struct integer_object
{
    enum object_type type;
    long long value;
    char inspect[24];
};

struct boolean_object
{
    enum object_type type;
    bool value;
    char *inspect;
};

struct return_value
{
    enum object_type type;
    struct object *value;
};

struct null_object
{
    enum object_type type;
    char *inspect;
};

extern struct boolean_object true_object;
extern struct boolean_object false_object;
extern struct null_object null_object;

void object_destroy(struct object *object);
char *object_inspect(struct object *object);
struct integer_object *integer_object_alloc(long long value);
struct boolean_object *boolean_object_alloc(bool value);
struct return_value *return_value_alloc(struct object *value);
struct null_object *null_object_alloc(void);
#endif
