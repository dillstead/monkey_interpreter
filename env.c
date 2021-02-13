#include <assert.h>
#include <mem.h>

#include "util.h"
#include "env.h"
#include "ast.h"

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
    object_destroy((struct object *) *value);
}

void environment_addref(struct environment *env)
{
    env->cnt++;
}

struct environment *enclosed_environment_alloc(struct environment *outer)
{
    struct environment *env;

    env = environment_alloc();
    environment_addref(outer);
    env->outer = outer;
    return env;
}

struct environment *environment_alloc(void)
{
    struct environment *env;

    NEW0(env);
    env->cnt = 1;
    env->store = Table_new(0, text_cmp, text_hash);
    return env;
}

void environment_destroy(struct environment *env)
{
    if (env == NULL)
    {
        return;
    }
    if (--env->cnt > 0)
    {
        return;
    }
    environment_destroy(env->outer);
    Table_map(env->store, free_store, NULL);
    Table_free(&env->store);
    FREE(env);
}

struct object *environment_get(struct environment *env, Text_T name)
{
    struct object *value;
    
    value = (struct object *) Table_get(env->store, &name);
    if (value == NULL && env->outer != NULL)
    {
        value = environment_get(env->outer, name);
    }
    if (value != NULL)
    {
        object_addref(value);
    }
    return value;
}

struct object *environment_set(struct environment *env, Text_T name, struct object *value)
{
    struct object *prev;

    if (Table_get(env->store, &name) != NULL)
    {
        prev = (struct object *) Table_put(env->store, &name, value);
        object_destroy(prev);
    }
    else
    {
        Table_put(env->store, copy_text(name), value);
        
    }
    object_addref(value);
    return value;
}
