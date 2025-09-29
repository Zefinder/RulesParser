/*
 * This file defines the structures used to store the rules parsed by the parser. 
 */

#pragma once

#include "list.h"

/* The string representation of the ALLOW value */
#define ALLOW_STR "A"
/* The string representation of the DENY value */
#define DENY_STR "D"

/* The string representation of the integer type */
#define INT_STR "INT"
/* The string representation of the string type */
#define STRING_STR "STRING"

/* The type state_t hold the state's state number */
typedef unsigned int state_t;

/* 
 * The measure type holds an action performed by a stateless rule or by the action
 * (sometimes) in a stateful rule. There are two different measures: ALLOW and DENY. 
 */
typedef enum measure 
{
    ALLOW, DENY
} measure_t;

/*
 * An action, in a stateful rule, is the right operand of the stateful header 
 * (state|action ...). An action is either a measure or a state.
 */
typedef union action
{
    measure_t measure;
    state_t state;
} action_t;

/*
 * The value type holds the parameter value's type. There are two different types:
 * INT and STRING.
 */
typedef enum value_type
{
    INT, STRING
} value_type_t;

/*
 * The rule parameters type holds all the rule parameters' values. It holds the 
 * offset, the value type, the length and the parameter's value. Because a rule can 
 * exist without parameters, this type includes a is_present field to specify if 
 * the values inside the type are coherent.  
 */
typedef struct rule_parameters
{
    int is_present;
    unsigned int offset;
    value_type_t type;
    unsigned int length;
    char *value;
} rule_parameters_t;

/*
 * The rule body type is a type that is present in both stateless and stateful 
 * (sub)rules. It holds the message id, the function code and the parameters of
 * the (sub)rule. 
 */
typedef struct rule_body
{
    unsigned int message_id;
    unsigned int function_code;
    rule_parameters_t rule_parameters;
} rule_body_t;

/*
 * The stateless rule type represents a stateless rule. It holds a measure and a
 * rule body. 
 */
typedef struct stateless_rule
{
    measure_t measure;
    rule_body_t rule_body;
} stateless_rule_t;

/*
 * The stateful header type represents the header for a stateful rule (i.e. 
 * state|action). It holds the rule's start state and its action if the rule
 * matches. 
 * 
 * [Impl specific] Because the action is a union, a field is_measure_action is
 * added to specify whether the action is a measure or not.
 */
typedef struct stateful_header
{
    state_t start_state;
    action_t action;
    int is_measure_action;
} stateful_header_t;

/*
 * The stateful subrule type represents the part of a stateful rule (i.e. subrule 
 * ; subrule ; ...). It holds the stateful header and the rule body. 
 */
typedef struct stateful_subrule
{
    stateful_header_t stateful_header;
    rule_body_t rule_body;
} stateful_subrule_t;

/*
 * The stateful rule type represents all subrule of a rule. It is implemented as a
 * list of subrules.
 */
typedef struct stateful_rule
{
    list_t stateful_list;
} stateful_rule_t;

/*
 * A rule is either a stateless rule or a stateful rule. 
 */
typedef union rule
{
    stateless_rule_t stateless_rule;
    stateful_rule_t stateful_rule;
} rule_t;

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