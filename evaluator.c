#include "evaluator.h"

static struct object *eval_program(struct program *program, struct environment *env)
{
    struct object *object;
    struct return_value *return_value;

    for (int i = 0; i < Seq_length(program->statements); i++)
    {
        object = eval((struct node *) Seq_get(program->statements, i), env);
        if (object->type == RETURN_VALUE)
        {
            return_value = (struct return_value *) object;
            object = return_value->value;
            object_destroy((struct object *) return_value);
            return object;
        }
        else if (object->type == ERROR_OBJ)
        {
            return object;
        }
    }
    return object;
}

static struct object *eval_expression_statement(struct expression_statement *expression_statement,
                                                struct environment *env)
{
    return eval((struct node *) expression_statement->expression, env);    
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
        return (struct object *) error_object_alloc("unknown operator: -%s", 
                                                    object_type_str[right->type]);
    }
    value = ((struct integer_object *) right)->value;
    return (struct object *) integer_object_alloc(-value);
}

static struct object *eval_prefix_expression(struct prefix_expression *prefix_expression, struct environment *env)
{
    struct object *right = NULL;
    struct object *object = NULL;
    
    right = eval((struct node *) prefix_expression->right, env);
    if (right->type == ERROR_OBJ)
    {
        return right;
    }
    if (Text_cmp(prefix_expression->op, (Text_T) { sizeof "!" - 1, "!" }) == 0)
    {
        object = eval_bang_operator_expression(right);
    }
    else if (Text_cmp(prefix_expression->op, (Text_T) { sizeof "-" - 1, "-" }) == 0)
    {
        object = eval_minus_prefix_operator_expression(right);
    }
    else
    {
        object = (struct object *) error_object_alloc("unknown operator: %T%s", &prefix_expression->op,
                                                      object_type_str[right->type]);
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
        return (struct object *) error_object_alloc("unknown operator: %s %T %s", 
                                                    object_type_str[left->type],
                                                    &op,
                                                    object_type_str[right->type]);   
    }
}

static struct object *eval_infix_expression(struct infix_expression *infix_expression, struct environment *env)
{
    struct object *right = NULL;
    struct object *left = NULL;
    Text_T op = infix_expression->op;
    struct object *object = (struct object *) &null_object;

    left = eval((struct node *) infix_expression->left, env);
    if (left->type == ERROR_OBJ)
    {
        return left;
    }
    right = eval((struct node *) infix_expression->right, env);
    if (right->type == ERROR_OBJ)
    {
        object_destroy(left);
        return right;
    }
    if (left->type == INTEGER_OBJ && right->type == INTEGER_OBJ)
    {
        object = eval_integer_infix_expression(left, right, infix_expression->op);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "==" - 1, "==" }) == 0)
    {
        object = (struct object *) boolean_object_alloc(left == right);
    }
    else if (Text_cmp(op, (Text_T) { sizeof "!=" - 1, "!=" }) == 0)
    {
        object = (struct object *) boolean_object_alloc(left != right);
    }
    else if (left->type != right->type)
    {
        object = (struct object *) error_object_alloc("type mismatch: %s %T %s", 
                                                      object_type_str[left->type],
                                                      &op,
                                                      object_type_str[right->type]);   
    }
    else
    {
        object = (struct object *) error_object_alloc("unknown operator: %s %T %s", 
                                                      object_type_str[left->type],
                                                      &op,
                                                      object_type_str[right->type]);   
    }
    object_destroy(left);
    object_destroy(right);
    return object;
}

static struct object *eval_block_statement(struct block_statement *block_statement, struct environment *env)
{
    struct object *object;

    for (int i = 0; i < Seq_length(block_statement->statements); i++)
    {
        object = eval((struct node *) Seq_get(block_statement->statements, i), env);
        if (object->type == RETURN_VALUE || object->type == ERROR_OBJ)
        {
            return object;
        }
    }
    return object;
}

static struct object *eval_return_statement(struct return_statement *return_statement, struct environment *env)
{
    struct object *value;
    
    value = eval((struct node *) return_statement->return_value, env);
    if (value->type == ERROR_OBJ)
    {
        return value;
    }
    return (struct object *) return_value_alloc(value);    
}

static struct object *eval_let_statement(struct let_statement *let_statement, struct environment *env)
{
    struct object *value;
    
    value = eval((struct node *) let_statement->value, env);
    if (value->type == ERROR_OBJ)
    {
        return value;
    }
    environment_set(env, let_statement->name->value, value);
    return (struct object *) &null_object;
}

static struct object *eval_identifier(struct identifier *identifier, struct environment *env)
{
    struct object *value;
    
    value = environment_get(env, identifier->value);
    if (value == NULL)
    {
        return (struct object *) error_object_alloc("identifier not found: %T", 
                                                    &identifier->value);
    }
    return value;
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

static struct object *eval_if_expression(struct if_expression *if_expression, struct environment *env)
{
    struct object *condition;
    struct object *object =  (struct object *) &null_object;
    
    condition = eval((struct node *) if_expression->condition, env);
    if (condition->type == ERROR_OBJ)
    {
        return condition;
    }
    if (is_truthy(condition))
    {
        object = eval((struct node *) if_expression->consequence, env);
    }
    else if (if_expression->alternative != NULL)
    {
        object = eval((struct node *) if_expression->alternative, env);
    }
    object_destroy(condition);
    return object;
}
    
struct object *eval(struct node *node, struct environment *env)
{
    switch (node->type)
    {
    case PROGRAM:
    {
        return eval_program((struct program *) node, env);
    }
    case BLOCK_STMT:
    {
        return eval_block_statement((struct block_statement *) node, env);
    }
    case IF_EXPR:
    {
        return eval_if_expression((struct if_expression *) node, env);
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
        return eval_expression_statement((struct expression_statement *) node, env);
    }
    case PREFIX_EXPR:
    {
        return eval_prefix_expression((struct prefix_expression *) node, env);
    }
    case INFIX_EXPR:
    {
        return eval_infix_expression((struct infix_expression *) node, env);
    }
    case RETURN_STMT:
    {
        return eval_return_statement((struct return_statement *) node, env);
    }
    case LET_STMT:
    {
        return eval_let_statement((struct let_statement *) node, env);
    }
    case IDENT_EXPR:
    {
        return eval_identifier((struct identifier *) node, env);
    }
    default:
    {
        return (struct object *) &null_object;
    }
    }
    return (struct object *) &null_object;
}
