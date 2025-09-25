#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rules.h"
#include "list.h"

void remove_rule_handler(void *user_data)
{
    node_data_t *node_data = (node_data_t *) user_data;

    if (node_data->is_stateful)
    {
        list_t *stateful_rules_ptr = &node_data->rule.stateful_rule.stateful_list;
        int stateful_subrules_number = stateful_rules_ptr->list_size;
        for (int i = 0; i < stateful_subrules_number; i++)
        {
            // Remove the subrule one by one 
            remove_first(stateful_rules_ptr);
        }
    } else 
    {
        stateless_rule_t stateless_rule = node_data->rule.stateless_rule;
        if (stateless_rule.rule_body.rule_parameters.is_present)
        {
            free(stateless_rule.rule_body.rule_parameters.value);
        }
    }
    
    free(node_data);
}

void remove_stateful_subrule_handler(void *user_data)
{
    stateful_subrule_t *subrule = (stateful_subrule_t *) user_data;
    if (subrule->rule_body.rule_parameters.is_present)
    {
        free(subrule->rule_body.rule_parameters.value);
    }

    free(subrule);
}

list_t rule_list = {.list_size = 0, .first_node = NULL, .last_node = NULL, .remove_handler = &remove_rule_handler};
list_t *stateful_rules_ptr;

rule_body_t rule_body;
rule_parameters_t rule_parameters;
stateful_header_t stateful_header;
value_type_t value_type;

void create_rule_body(unsigned int message_id, unsigned int function_code)
{
    rule_body.message_id = message_id;
    rule_body.function_code = function_code;
    rule_body.rule_parameters = rule_parameters;
}

void create_rule_parameters(unsigned int offset, unsigned int length, char* value)
{
    rule_parameters.is_present = 1;
    rule_parameters.offset = offset;
    rule_parameters.type = value_type;
    rule_parameters.length = length;
    rule_parameters.value = strdup(value);
}

void create_stateful_header(state_t start_state, action_t action, int is_measure_action)
{
    stateful_header.start_state = start_state;
    stateful_header.action = action;
    stateful_header.is_measure_action = is_measure_action;
}

void set_parameter_value_type(value_type_t type)
{
    value_type = type;
}

value_type_t get_parameter_value_type(void)
{
    return value_type;
}

void add_stateless_rule(measure_t measure)
{
    node_data_t *node_data = malloc(sizeof(node_data_t));
    node_data->is_stateful = 0;
    stateless_rule_t* stateless_rule = &node_data->rule.stateless_rule;
    stateless_rule->measure = measure;
    stateless_rule->rule_body = rule_body;
    add_node(&rule_list, node_data);

    // Reset parameters presence
    rule_parameters.is_present = 0;
}

void add_stateful_rule()
{
    node_data_t *node_data = malloc(sizeof(node_data_t));
    node_data->is_stateful = 1;
    node_data->rule.stateful_rule.stateful_list = *stateful_rules_ptr;
    add_node(&rule_list, node_data);

    // Free the list pointer since the content has been copied
    free(stateful_rules_ptr);
    stateful_rules_ptr = NULL; 
}

void add_stateful_subrule()
{
    if (stateful_rules_ptr == NULL)
    {
        // Init the stateful rules list
        stateful_rules_ptr = malloc(sizeof(list_t));
        stateful_rules_ptr->list_size = 0;
        stateful_rules_ptr->first_node = NULL;
        stateful_rules_ptr->last_node = NULL;
        stateful_rules_ptr->remove_handler = &remove_stateful_subrule_handler;
    }

    stateful_subrule_t* stateful_subrule = malloc(sizeof(stateful_subrule_t));
    stateful_subrule->stateful_header = stateful_header;
    stateful_subrule->rule_body = rule_body;
    add_node(stateful_rules_ptr, stateful_subrule);

    // Reset parameters presence
    rule_parameters.is_present = 0;
}

void print_rule_parameters(rule_parameters_t rule_parameters)
{
    if (rule_parameters.is_present)
    {
        printf(" %d:%s:%d %s", rule_parameters.offset, rule_parameters.type == INT ? INT_STR : STRING_STR, rule_parameters.length, rule_parameters.value);
    }
}

void print_rule_body(rule_body_t rule_body)
{
    printf(" %d %d", rule_body.message_id, rule_body.function_code);
    print_rule_parameters(rule_body.rule_parameters);
}

void print_rules(void)
{
    if (rule_list.list_size == 0)
    {
        printf("Empty rule list\n");
    } else
    {
        for (int i = 0; i < rule_list.list_size; i++)
        {
            node_data_t *node_data = get(&rule_list, i);
            if (node_data->is_stateful)
            {
                list_t *stateful_rules_ptr = &node_data->rule.stateful_rule.stateful_list;
                int stateful_rules_number = stateful_rules_ptr->list_size;

                for (int i = 0; i < stateful_rules_number; i++)
                {
                    stateful_subrule_t *stateful_subrule = get(stateful_rules_ptr, i);
                    
                    printf("E%d|", stateful_subrule->stateful_header.start_state);
                    
                    if (stateful_subrule->stateful_header.is_measure_action)
                    {
                        printf("%s", stateful_subrule->stateful_header.action.measure == ALLOW ? ALLOW_STR : DENY_STR);
                    } else 
                    {
                        printf("E%d", stateful_subrule->stateful_header.action.state);
                    }
                    
                    print_rule_body(stateful_subrule->rule_body);

                    if (i != stateful_rules_number - 1)
                    {
                        printf(" ; ");
                    }
                }
            } else 
            {
                stateless_rule_t *stateless_rule = &node_data->rule.stateless_rule;
                printf("%s", stateless_rule->measure == ALLOW ? ALLOW_STR : DENY_STR);
                print_rule_body(stateless_rule->rule_body);
            }

            printf("\n");
        }
    }
}

rule_t get_rule(int index)
{
    return *((rule_t *) (get(&rule_list, index)));
}

int remove_rule(int index)
{
    return remove_node(&rule_list, index);
}

// int main()
// {
//     print_rules();
//     printf("\n");

//     char* value;
//     action_t action;
//     action.state = 1;
//     create_rule_body(0x0852, 45);
//     create_stateful_header(0, action, 0);
//     add_stateful_subrule();

//     value = strdup("10");
//     create_rule_parameters(4, 7, value);
//     create_rule_body(0x0974, 7);
//     create_stateful_header(1, action, 1);
//     add_stateful_subrule();
    
//     create_rule_body(0x0846, 41);
//     action.measure = ALLOW;
//     create_stateful_header(0, action, 1);
//     add_stateful_subrule();
//     add_stateful_rule();

//     value = strdup("AAAA");
//     create_rule_parameters(7, 1, value);
//     create_rule_body(0x0900, 9);
//     add_stateless_rule(ALLOW);

//     print_rules();
//     printf("\n");

//     remove_rule(2);

//     print_rules();
// }