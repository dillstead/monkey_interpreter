#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <text.h>

enum object_type
{
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    RETURN_VALUE,
    NULL_OBJ,
    ERROR_OBJ
};

extern const char *object_type_str[];

struct object
{
    enum object_type type;
    unsigned int cnt;
};

struct integer_object
{
    enum object_type type;
    unsigned int cnt;
    long long value;
    char inspect[24];
};

struct boolean_object
{
    enum object_type type;
    unsigned int cnt;
    bool value;
    char *inspect;
};

struct return_value
{
    enum object_type type;
    unsigned int cnt;
    struct object *value;
};

struct null_object
{
    enum object_type type;
    unsigned int cnt;
    char *inspect;
};

struct error_object
{
    enum object_type type;
    unsigned int cnt;
    char value[128];
};

extern struct boolean_object true_object;
extern struct boolean_object false_object;
extern struct null_object null_object;

void object_addref(struct object *object);
void object_delref(struct object *object);
void object_destroy(struct object *object);
char *object_inspect(struct object *object);
struct integer_object *integer_object_alloc(long long value);
struct boolean_object *boolean_object_alloc(bool value);
struct return_value *return_value_alloc(struct object *value);
struct null_object *null_object_alloc(void);
struct error_object *error_object_alloc(const char *value, ...);
#endif
