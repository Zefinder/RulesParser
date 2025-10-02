/*
 * This file includes all useful methods to write rules in a CSV format.
 */

#pragma once

#include <stdio.h>
#include <rules.h>

/*
 * Creates a CSV file. After calling this, all functions of this file
 * are callable.
 */
int create_csv(void);

/*
 * Writes a rule in the CSV file. This requires that the create_csv
 * function has been called.
 */
void write_rule(rule_t);

/*
 * Closes the CSV file
 */
void close_csv(void);