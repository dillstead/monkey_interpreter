#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast.h"
#include "object.h"
#include "env.h"

struct object *eval(struct node *node, struct environment *env);

#endif
