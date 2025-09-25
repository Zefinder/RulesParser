#pragma once

#include "list.h"

#define ALLOW_STR "A"
#define DENY_STR "D"

#define INT_STR "INT"
#define STRING_STR "STRING"

typedef unsigned int state_t;

typedef enum measure 
{
    ALLOW, DENY
} measure_t;

typedef union action
{
    measure_t measure;
    state_t state;
} action_t;

typedef enum value_type
{
    INT, STRING
} value_type_t;

typedef struct rule_parameters
{
    int is_present;
    unsigned int offset;
    value_type_t type;
    unsigned int length;
    char *value;
} rule_parameters_t;

typedef struct rule_body
{
    unsigned int message_id;
    unsigned int function_code;
    rule_parameters_t rule_parameters;
} rule_body_t;

typedef struct stateless_rule
{
    measure_t measure;
    rule_body_t rule_body;
} stateless_rule_t;

typedef struct stateful_header
{
    state_t start_state;
    action_t action;
    int is_measure_action;
} stateful_header_t;

typedef struct stateful_subrule
{
    stateful_header_t stateful_header;
    rule_body_t rule_body;
} stateful_subrule_t;

typedef struct stateful_rule
{
    list_t stateful_list;
    // action_t action;
    // int is_measure_action;
    // rule_body_t rule_body;
} stateful_rule_t;

typedef union rule
{
    stateless_rule_t stateless_rule;
    stateful_rule_t stateful_rule;
} rule_t;

typedef struct node_data
{
    rule_t rule;
    int is_stateful;
} node_data_t;

/*
 * Creates a rule body, this needs to be called after create_rule_parameters.
 */
void create_rule_body(unsigned int, unsigned int);

/* 
 * Creates rule parameters. It will duplicate the char * and will be automatically 
 * freed when the attached rule will be removed. 
 */
void create_rule_parameters(unsigned int, unsigned int, char*);

/*
 * Create a stateful header for a stateful subrule
 */
void create_stateful_header(state_t, action_t, int);

/*
 * Sets the parameter value type 
 */
void set_parameter_value_type(value_type_t);

/*
 * Returns the parameter value type
 */
value_type_t get_parameter_value_type(void);

/*
 * Adds a stateless rule. This needs to be called after create_rule_body. This
 * rule will be added to a rule list. You can access this list using the get_rule
 * and remove_rule. 
 */
void add_stateless_rule(measure_t);

/*
 * Adds a subrule for the current stateful rule.
 */
void add_stateful_subrule(void);

/*
 * Adds a stateful rule to the current list of rules. A stateful rule is composed 
 * of stateful subrules. A subrule is for example E0|D 0x0821 10, and subrules are 
 * separed by semicolons. 
 */
void add_stateful_rule(void);

/*
 * Prints all added rules.
 */
void print_rules(void);

/*
 * Gets the rule at the specified index.
 */
rule_t get_rule(int);

/*
 * Removes the rule at the specified index.
 */
int remove_rule(int);