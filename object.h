#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <text.h>
#include <seq.h>

#include "env.h"

enum object_type
{
    INTEGER_OBJ,
    BOOLEAN_OBJ,
    STRING_OBJ,
    ARRAY_OBJ,
    HASH_OBJ,
    FUNC_OBJ,
    BUILTIN_OBJ,
    RETURN_VALUE_OBJ,
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

struct string_object
{
    enum object_type type;
    unsigned int cnt;
    char *value;
};

struct array_object
{
    enum object_type type;
    unsigned int cnt;
    Seq_T elements;
    char *inspect;
};

struct hash_object
{
    enum object_type type;
    unsigned int cnt;
    Table_T pairs;
    char *inspect;
};

struct function_object
{
    enum object_type type;
    unsigned int cnt;
    struct environment *env;
    struct function_literal *value;
    char *inspect;
};

struct builtin_object
{
    enum object_type type;
    unsigned int cnt;
    struct object *(*value)(Seq_T args);
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

static inline bool is_object_hash_key(struct object *object)
{
    return object->type == INTEGER_OBJ
        || object->type == BOOLEAN_OBJ
        || object->type == STRING_OBJ;
}
int object_cmp(const void *x, const void *y);
unsigned object_hash(const void *x);
void object_addref(struct object *object);
void object_destroy(struct object *object);
char *object_inspect(struct object *object);
struct integer_object *integer_object_alloc(long long value);
struct boolean_object *boolean_object_alloc(bool value);
struct string_object *string_object_alloc(Text_T value);
struct array_object *array_object_alloc(Seq_T elements);
struct hash_object *hash_object_alloc(Table_T pairs);
struct function_object *function_object_alloc(struct function_literal *value,
                                              struct environment *env);
struct return_value *return_value_alloc(struct object *value);
struct null_object *null_object_alloc(void);
struct error_object *error_object_alloc(const char *value, ...);
void free_hash_pairs(const void *key, void **value, void *cl);
#endif
