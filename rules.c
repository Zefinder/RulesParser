#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rules.h"
#include "list.h"

/*
 * The node data type holds the rule and its statefulness to store it in the rule list.
 */
typedef struct node_data
{
    subrule_t subrule;
} node_data_t;

void remove_rule_handler(void *user_data)
{
    node_data_t *node_data = (node_data_t *) user_data;
    subrule_t subrule = node_data->subrule;
    if (subrule.rule_body.rule_parameters.value == NULL)
    {
        free(subrule.rule_body.rule_parameters.value);
    }

    free(node_data);
}

list_t rule_list = {.list_size = 0, .first_node = NULL, .last_node = NULL, .remove_handler = &remove_rule_handler};

unsigned int rule_id;

rule_body_t rule_body;
rule_parameters_t rule_parameters;
header_t header;
value_type_t value_type;

void create_rule_body(unsigned int message_id, unsigned int function_code)
{
    rule_body.message_id = message_id;
    rule_body.function_code = function_code;
    rule_body.rule_parameters = rule_parameters;
}

void create_rule_parameters(unsigned int offset, unsigned int length, char* value)
{
    rule_parameters.offset = offset;
    rule_parameters.length = length;
    rule_parameters.value = strdup(value);
}

void create_stateful_header(state_t start_state, action_t action)
{
    header.start_state = start_state;
    header.action = action;
}

void set_parameter_value_type(value_type_t type)
{
    value_type = type;
}

value_type_t get_parameter_value_type(void)
{
    return value_type;
}

void add_subrule()
{
    node_data_t *node_data = malloc(sizeof(node_data_t));
    subrule_t *subrule = &node_data->subrule;

    subrule->rule_id = rule_id;
    subrule->header = header;
    subrule->rule_body = rule_body;
    add_node(&rule_list, node_data);

    // Reset parameters
    rule_parameters.raw[0] = 0;
    rule_parameters.raw[1] = 0;
    rule_parameters.raw[2] = 0;

    // Reset rule body
    rule_body.long_raw[0] = 0;
    rule_body.long_raw[1] = 0;
}

void add_stateless_rule(measure_t measure)
{
    create_stateful_header(0, (action_t) measure);
    add_subrule();

    // Increment rule id
    rule_id++;
}

void add_stateful_rule()
{
    // Increment rule id
    rule_id++;
}

void print_rule_parameters(rule_parameters_t rule_parameters)
{
    if (rule_parameters.value != NULL)
    {
        printf(" %d %d %s", rule_parameters.offset, rule_parameters.length, rule_parameters.value);
    }
}

void print_rule_body(rule_body_t rule_body)
{
    printf(" %d %d", rule_body.message_id, rule_body.function_code);
}

void print_header(header_t header)
{
    printf("E%d|", header.start_state);
    if (header.action.measure == ALERT || header.action.measure == DROP)
    {
        printf("%s", header.action.measure == ALERT ? ALLOW_STR : DENY_STR);
    } else 
    {
        printf("E%d", header.action.state);
    }
}

void print_rules(void)
{
    if (rule_list.list_size == 0)
    {
        printf("Empty rule list\n");
    } else
    {
        printf("List size: %d\n", rule_list.list_size);
        for (int i = 0; i < rule_list.list_size; i++)
        {
            node_data_t *node_data = get(&rule_list, i);
            subrule_t subrule = node_data->subrule;
            printf("(%d) -> ", subrule.rule_id);
            print_header(subrule.header);
            print_rule_body(subrule.rule_body);
            print_rule_parameters(subrule.rule_body.rule_parameters);
            
            printf("\n");
        }
    }
}

int get_number_of_rules(void)
{
    return rule_list.list_size;
}

subrule_t *get_rule(int index)
{
    node_data_t *node_data = get(&rule_list, index);
    subrule_t *subrule = &node_data->subrule;
    return &node_data->subrule;
}

int remove_rule(int index)
{
    return remove_node(&rule_list, index);
}

// int main()
// {
//     subrule_t subrule = {.header={.start_state=1, .action=2}, .rule_id=0, .rule_body={.message_id=0x1840, .function_code=32, .rule_parameters.raw={0, 0, 0}}};
//     // subrule_t subrule = {.raw = {0x00000102, 0x18400020, 0, 0, 0}};
//     printf("(%d) -> ", subrule.rule_id);
//     print_header(subrule.header);
//     print_rule_body(subrule.rule_body);
//     print_rule_parameters(subrule.rule_body.rule_parameters);
//     printf("\n");
//     printf("{0x%08X,", subrule.raw[0]);
//     printf(" 0x%08X,", subrule.raw[1]);
//     printf(" 0x%08X,", subrule.raw[2]);
//     printf(" 0x%08X,", subrule.raw[3]);
//     printf(" 0x%08X}\n", subrule.raw[4]);

//     return 0;
// }