#include <seq.h>
#include <str.h>

#include "builtins.h"
#include "util.h"

static Table_T builtins;

static struct object *len(Seq_T args)
{
    struct object *arg;
    struct string_object *string_object;
    struct array_object *array_object;
    
    if (Seq_length(args) != 1)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=1",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type == STRING_OBJ)
    {
        string_object = (struct string_object *) arg;
        return (struct object *) integer_object_alloc(Str_len(string_object->value, 1, 0));

    }
    else if (arg->type == ARRAY_OBJ)
    {
        array_object = (struct array_object *) arg;
        return (struct object *) integer_object_alloc(Seq_length(array_object->elements));
    }
    return (struct object *) error_object_alloc("argument to 'len' not supported, got %s",
                                                    object_type_str[arg->type]);        

}

static struct object *first(Seq_T args)
{
    struct object *arg;
    struct array_object *array_object;
    struct object *object;
    
    if (Seq_length(args) != 1)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=1",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type == ARRAY_OBJ)
    {
        array_object = (struct array_object *) arg;
        if (Seq_length(array_object->elements) > 0)
        {
            object = Seq_get(array_object->elements, 0);
            object_addref(object);
            return object;
        }
        return (struct object *) &null_object;
    }
    return (struct object *) error_object_alloc("argument to 'first' must be ARRAY, got %s",
                                                object_type_str[arg->type]);
}

static struct object *last(Seq_T args)
{
    struct object *arg;
    struct array_object *array_object;
    struct object *object;
    
    if (Seq_length(args) != 1)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=1",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type == ARRAY_OBJ)
    {
        array_object = (struct array_object *) arg;
        if (Seq_length(array_object->elements) > 0)
        {
            object = Seq_get(array_object->elements, Seq_length(array_object->elements) - 1);
            object_addref(object);
            return object;
        }
        return (struct object *) &null_object;
    }
    return (struct object *) error_object_alloc("argument to 'last' must be ARRAY, got %s",
                                                object_type_str[arg->type]);
}

static struct object *rest(Seq_T args)
{
    struct object *arg;
    struct array_object *array_object;
    struct object *object;
    Seq_T elements;
    
    if (Seq_length(args) != 1)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=1",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type == ARRAY_OBJ)
    {
        array_object = (struct array_object *) arg;
        if (Seq_length(array_object->elements) > 0)
        {
            elements = Seq_new(Seq_length(array_object->elements) - 1);
            for (int i = 1; i < Seq_length(array_object->elements); i++)
            {
                object = (struct object *) Seq_get(array_object->elements, i);
                object_addref(object);
                Seq_addhi(elements, object);
            }
            return (struct object *) array_object_alloc(elements);
        }
        return (struct object *) &null_object;
    }
    return (struct object *) error_object_alloc("argument to 'rest' must be ARRAY, got %s",
                                                object_type_str[arg->type]);
}

static struct object *push(Seq_T args)
{
    struct object *arg;
    struct array_object *array_object;
    struct object *object;
    Seq_T elements;
    
    if (Seq_length(args) != 2)
    {
        return (struct object *) error_object_alloc("wrong number of arguments. got=%d, want=2",
                                                    Seq_length(args));
    }
    arg = (struct object *) Seq_get(args, 0);
    if (arg->type == ARRAY_OBJ)
    {
        array_object = (struct array_object *) arg;
        elements = Seq_new(Seq_length(array_object->elements) + 1);
        for (int i = 0; i < Seq_length(array_object->elements); i++)
        {
            object = (struct object *) Seq_get(array_object->elements, i);
            object_addref(object);
            Seq_addhi(elements, object);
        }
        object = (struct object *) Seq_get(args, 1);
        object_addref(object);
        Seq_addhi(elements, object);
        return (struct object *) array_object_alloc(elements);
    }
    return (struct object *) error_object_alloc("first argument to 'push' must be ARRAY, got %s",
                                                object_type_str[arg->type]);
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
          { {sizeof "len" - 1, "len"}, {BUILTIN_OBJ, 1, len} },
          { {sizeof "first" - 1, "first"}, {BUILTIN_OBJ, 1, first} },
          { {sizeof "last" - 1, "last"}, {BUILTIN_OBJ, 1, last} },
          { {sizeof "rest" - 1, "rest"}, {BUILTIN_OBJ, 1, rest} },
          { {sizeof "push" - 1, "push"}, {BUILTIN_OBJ, 1, push} }
      };
    builtins = Table_new(0, text_cmp, text_hash);
    for (int i = 0; i < sizeof identifier_builtins / sizeof identifier_builtins[0]; i++)
    {
        Table_put(builtins, &identifier_builtins[i].identifier,
                  &identifier_builtins[i].builtin);
    }
}
