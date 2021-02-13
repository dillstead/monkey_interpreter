#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <table.h>
#include <text.h>

#include "object.h"

struct environment
{
    unsigned int cnt;
    Table_T store;
    struct environment *outer;
};
    
struct environment *environment_alloc(void);
struct environment *enclosed_environment_alloc(struct environment *outer);
void environment_addref(struct environment *env);
void environment_destroy(struct environment *env);
struct object *environment_get(struct environment *env, Text_T name);
struct object *environment_set(struct environment *env, Text_T name, struct object *value);

#endif
