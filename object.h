#ifndef OBJECT_H
#define OBJECT_H

#include <stdbool.h>
#include <table.h>
#include <text.h>
#include <seq.h>

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
    ENV_OBJ,
    NULL_OBJ,
    ERROR_OBJ
};

extern const char *object_type_str[];

struct object
{
    enum object_type type;
    bool marked;
};

struct env_object
{
    enum object_type type;
    bool marked;
    Table_T store;
    struct env_object *outer;
};

struct integer_object
{
    enum object_type type;
    bool marked;
    long long value;
    char inspect[24];
};

struct boolean_object
{
    enum object_type type;
    bool marked;
    bool value;
    char *inspect;
};

struct string_object
{
    enum object_type type;
    bool marked;
    char *value;
};

struct array_object
{
    enum object_type type;
    bool marked;
    Seq_T elements;
    char *inspect;
};

struct hash_object
{
    enum object_type type;
    bool marked;
    Table_T pairs;
    char *inspect;
};

struct function_object
{
    enum object_type type;
    bool marked;
    struct env_object *env;
    struct function_literal *value;
    char *inspect;
};

struct builtin_object
{
    enum object_type type;
    bool marked;
    struct object *(*value)(Seq_T args);
};

struct return_value
{
    enum object_type type;
    bool marked;
    struct object *value;
};

struct null_object
{
    enum object_type type;
    bool marked;
    char *inspect;
};

struct error_object
{
    enum object_type type;
    bool marked;
    char value[128];
};

extern struct boolean_object true_object;
extern struct boolean_object false_object;
extern struct null_object null_object;

void objects_init(void);
void objects_gc(struct env_object *env);
void objects_destroy(void);
static inline bool is_object_hash_key(struct object *object)
{
    return object->type == INTEGER_OBJ
        || object->type == BOOLEAN_OBJ
        || object->type == STRING_OBJ;
}
int object_cmp(const void *x, const void *y);
unsigned object_hash(const void *x);
char *object_inspect(struct object *object);
struct integer_object *integer_object_alloc(long long value);
struct boolean_object *boolean_object_alloc(bool value);
struct string_object *string_object_alloc(Text_T value);
struct array_object *array_object_alloc(Seq_T elements);
struct hash_object *hash_object_alloc(Table_T pairs);
struct function_object *function_object_alloc(struct function_literal *value,
                                              struct env_object *env);
struct return_value *return_value_alloc(struct object *value);
struct error_object *error_object_alloc(const char *value, ...);
struct env_object *env_object_alloc(struct env_object *outer);
struct object *env_get(struct env_object *env, Text_T name);
struct object *env_set(struct env_object *env, Text_T name, struct object *value);
void free_hash_pairs(const void *key, void **value, void *cl);
#endif
