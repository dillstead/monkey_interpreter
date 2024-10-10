#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "object.h"

struct object *eval(struct node *node, struct env_object *env);

#endif
