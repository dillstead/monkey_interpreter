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
    long long value;
    
    if (right->type != INTEGER_OBJ)
    {
        return (struct object *) &null_object;
    }
    value = ((struct integer_object *) right)->value;
    return (struct object *) integer_object_alloc(-value);
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

static struct object *eval_integer_infix_expression(struct object *left, struct object *right, Text_T op)
{
    long long left_value;
    long long right_value;

    left_value = ((struct integer_object *) left)->value;
    right_value = ((struct integer_object *) right)->value;
    if (Text_cmp(op, (Text_T) { sizeof "+" - 1, "+" }) == 0)
    {
        return (struct object *) integer_object_alloc(left_value + right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "-" - 1, "-" }) == 0)
    {
        return (struct object *) integer_object_alloc(left_value - right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "*" - 1, "*" }) == 0)
    {
        return (struct object *) integer_object_alloc(left_value * right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "/" - 1, "/" }) == 0)
    {
        return (struct object *) integer_object_alloc(left_value / right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof ">" - 1, ">" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left_value > right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "<" - 1, "<" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left_value < right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "==" - 1, "==" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left_value == right_value);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "!=" - 1, "!=" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left_value != right_value);
    }
    else
    {
        return (struct object *) &null_object;
    }
}

static struct object *eval_infix_expression(struct infix_expression *infix_expression)
{
    struct object *right;
    struct object *left;
    Text_T op = infix_expression->op;
    struct object *object = (struct object *) &null_object;

    left = eval((struct node *) infix_expression->left);
    right = eval((struct node *) infix_expression->right);
    if (left->type == INTEGER_OBJ && right->type == INTEGER_OBJ)
    {
        object = eval_integer_infix_expression(left, right, infix_expression->op);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "==" - 1, "==" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left == right);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "!=" - 1, "!=" }) == 0)
    {
        return (struct object *) boolean_object_alloc(left != right);
    }
    else
    {
        object = (struct object *) &null_object;
    }
    object_destroy(left);
    object_destroy(right);
    return object;
}

static struct object *eval_block_statement(struct block_statement *block_statement)
{
    struct object *object;

    for (int i = 0; i < Seq_length(block_statement->statements); i++)
    {
        object = eval((struct node *) Seq_get(block_statement->statements, i));
    }
    return object;
}

static bool is_truthy(struct object *object)
{
    if (object == (struct object *) &true_object)
    {
        return true;
    }
    else if (object == (struct object *) &false_object)
    {
        return false;
    }
    else if (object == (struct object *) &null_object)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static struct object *eval_if_expression(struct if_expression *if_expression)
{
    struct object *condition;
    struct object *object = (struct object *) &null_object;
    
    condition = eval((struct node *) if_expression->condition);
    if (is_truthy(condition))
    {
        object = eval((struct node *) if_expression->consequence);
    }
    else if (if_expression->alternative != NULL)
    {
        object = eval((struct node *) if_expression->alternative);
    }
    object_destroy(condition);
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
    case BLOCK_STMT:
    {
        return eval_block_statement((struct block_statement *) node);
    }
    case IF_EXPR:
    {
        return eval_if_expression((struct if_expression *) node);
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
    case INFIX_EXPR:
    {
        return eval_infix_expression((struct infix_expression *) node);
    }
    default:
    {
        return NULL;
    }
    }
    return NULL;
}




