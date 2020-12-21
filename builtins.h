#ifndef BUILTINS_H
#define BUILTINS_H

#include <table.h>

#include "ast.h"
#include "object.h"

void builtins_init(void);
struct object *builtins_get(struct identifier *identifier);

#endif
