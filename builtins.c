#include <seq.h>
#include <str.h>

#include "builtins.h"
#include "util.h"

static Table_T builtins;

static struct object *len(Seq_T args)
{
    struct object *arg;
    struct string_object *string_object;
    
    if (Seq_length(args) != 1)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=1",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type != STRING_OBJ)
    {
        return (struct object *) error_object_alloc("argument to 'len' not supported, got %s",
                                                    object_type_str[arg->type]);
    }
    string_object = (struct string_object *) arg;
    return (struct object *) integer_object_alloc(Str_len(string_object->value, 1, 0));
}

struct object *builtins_get(struct identifier *identifier)
{
    struct object *object;
    
    object = (struct object *) Table_get(builtins, &identifier->value);
    if (object != NULL)
    {
        object->cnt++;
    }
    return object;
}

void builtins_init(void)
{
    static struct identifier_builtin
    {
        Text_T identifier;
        struct builtin_object builtin;
    } identifier_builtins[] =
      {
          { {sizeof "len" - 1, "len"}, {BUILTIN_OBJ, 1, len} }
      };
    builtins = Table_new(0, text_cmp, text_hash);
    for (int i = 0; i < sizeof identifier_builtins / sizeof identifier_builtins[0]; i++)
    {
        Table_put(builtins, &identifier_builtins[i].identifier,
                  &identifier_builtins[i].builtin);
    }
}
