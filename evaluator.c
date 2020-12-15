#include <str.h>

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
        else if (i < Seq_length(program->statements) - 1)
        {
            object_destroy(object);
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

static struct object *eval_string_literal(struct string_literal *string_literal)
{
    return (struct object *) string_object_alloc(string_literal->value);
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

static struct object *eval_string_infix_expression(struct object *left, struct object *right, Text_T op)
{
    char *left_value;
    char *right_value;
    char *value;

    left_value = ((struct string_object *) left)->value;
    right_value = ((struct string_object *) right)->value;
    if (Text_cmp(op, (Text_T) { sizeof "+" - 1, "+" }) == 0)
    {
        value = Str_cat(left_value, 1, 0, right_value, 1, 0);
        return (struct object *) string_object_alloc(Text_box(value, Str_len(value, 1, 0)));
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
    else if (left->type == STRING_OBJ && right->type == STRING_OBJ)
    {
        object = eval_string_infix_expression(left, right, infix_expression->op);
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
        if (i < Seq_length(block_statement->statements) - 1)
        {
            object_destroy(object);
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

static struct object *eval_function_literal(struct function_literal *function_literal,
                                            struct environment *env)
{
    return (struct object *) function_object_alloc(function_literal, env);
}

Seq_T eval_expressions(Seq_T args, struct environment *env)
{
    struct object *evaluated;
    struct object *removed;
    Seq_T result;

    result = Seq_new(Seq_length(args));
    for (int i = 0; i < Seq_length(args); i++)
    {
        evaluated = eval((struct node *) Seq_get(args, i), env);
        if (evaluated->type == ERROR_OBJ)
        {
            while (Seq_length(result) > 0)
            {
                removed = (struct object *) Seq_remlo(result);
                object_destroy(removed);
            }
            Seq_addhi(result, evaluated);
            return result;
        }
        Seq_addhi(result, evaluated);
    }
    return result;
}

static struct environment *extend_function_env(struct function_object *function, Seq_T args)
{
    struct environment *env;
    struct identifier *param;

    env = enclosed_environment_alloc(function->env);
    for (int i = 0; i < Seq_length(function->value->parameters); i++)
    {
        param = (struct identifier *) Seq_get(function->value->parameters, i);
        environment_set(env, param->value, (struct object *) Seq_get(args, i));
    }
    return env;
}

struct object *unwrap_return_value(struct object *object)
{
    struct return_value *return_value;
    
    if (object->type == RETURN_VALUE)
    {
        return_value = (struct return_value *) object;
        object = return_value->value;
        object_destroy((struct object *) return_value);
        return object;
    }
    return object;
}

static struct object *apply_function(struct object *object, Seq_T args)
{
    struct environment *env;
    struct function_object *function;
    struct object *evaluated;
    
    if (object->type != FUNC_OBJ)
    {
        return (struct object *) error_object_alloc("not a function: %s", 
                                                    object_type_str[object->type]);
    }
    function = (struct function_object *) object;
    env = extend_function_env(function, args);
    evaluated = eval((struct node *) function->value->body, env);
    environment_destroy(env);
    return unwrap_return_value(evaluated);
}

static struct object *eval_call_expression(struct call_expression *call_expression,
                                           struct environment *env)
{
    struct object *object;
    struct object *arg;
    struct object *evaluated;
    Seq_T args;
    
    object = eval((struct node *) call_expression->function, env);
    if (object->type == ERROR_OBJ)
    {
        return object;
    }
    args = eval_expressions(call_expression->arguments, env);
    if (Seq_length(args) == 1)
    {
        arg = (struct object *) Seq_get(args, 0);
        if (arg->type == ERROR_OBJ)
        {
            Seq_free(&args);
            object_destroy(object);
            return arg;
        }
    }
    evaluated = apply_function(object, args);
    for (int i = 0; i < Seq_length(args); i++)
    {
        object_destroy((struct object *) Seq_get(args, i));
    }
    Seq_free(&args);
    object_destroy(object);
    return evaluated;
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
    case STRING_LITERAL_EXPR:
    {
        return eval_string_literal((struct string_literal *) node);
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
    case FUNC_LITERAL_EXPR:
    {
        return eval_function_literal((struct function_literal *) node, env);
    }
    case CALL_EXPR:
    {
        return eval_call_expression((struct call_expression *) node, env);
    }
    default:
    {
        return (struct object *) &null_object;
    }
    }
    return (struct object *) &null_object;
}
