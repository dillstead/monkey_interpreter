#include "evaluator.h"

static struct object *eval_program(struct program *program)
{
    struct object *object;

    for (int i = 0; i < Seq_length(program->statements); i++)
    {
        object = eval((struct node *) Seq_get(program->statements, i));
    }
    return object;
}

static struct object *eval_expression_statement(struct expression_statement *expression_statement)
{
    return eval((struct node *) expression_statement->expression);    
}

static struct object *eval_integer_literal(struct integer_literal *integer_literal)
{
    return (struct object *) integer_object_alloc(integer_literal->value);
}

static struct object *eval_boolean(struct boolean *boolean)
{
    return (struct object *) boolean_object_alloc(boolean->value);
}

static struct object *eval_bang_operator_expression(struct object *right)
{
    if (right == (struct object *) &true_object)
    {
        return (struct object *) &false_object;
    }
    else if (right == (struct object *) &false_object)
    {
        return (struct object *) &true_object;
    }
    else if (right == (struct object *) &null_object)
    {
        return (struct object *) &true_object;
    }
    else
    {
        return (struct object *) &false_object;
    }
}

static struct object *eval_minus_prefix_operator_expression(struct object *right)
{
    struct integer_object *integer_object;
    
    if (right->type != INTEGER_OBJ)
    {
        return (struct object *) &null_object;
    }
    integer_object = (struct integer_object *) right;
    return (struct object *) integer_object_alloc(-integer_object->value);
}

static struct object *eval_prefix_expression(struct prefix_expression *prefix_expression)
{
    struct object *right;
    struct object *object = NULL;
    
    right = eval((struct node *) prefix_expression->right);
    if (Text_cmp(prefix_expression->op, (Text_T) { sizeof "!" - 1, "!" }) == 0)
    {
        object = eval_bang_operator_expression(right);
    }
    else if (Text_cmp(prefix_expression->op, (Text_T) { sizeof "-" - 1, "-" }) == 0)
    {
        object = eval_minus_prefix_operator_expression(right);
    }
    object_destroy(right);
    return object;
}

struct object *eval(struct node *node)
{
    switch (node->type)
    {
    case PROGRAM:
    {
        return eval_program((struct program *) node);
    }
    case INT_LITERAL_EXPR:
    {
        return eval_integer_literal((struct integer_literal *) node);
    }
    case BOOL_EXPR:
    {
        return eval_boolean((struct boolean *) node);
    }
    case EXPR_STMT:
    {
        return eval_expression_statement((struct expression_statement *) node);
    }
    case PREFIX_EXPR:
    {
        return eval_prefix_expression((struct prefix_expression *) node);
    }
    default:
    {
        return NULL;
    }
    }
    return NULL;
}
