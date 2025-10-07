#pragma once

typedef enum ids_action
{
    SWITCH_STATE, ALERT_ACTION, DROP_ACTION, NOTHING
} ids_action_t;

typedef struct ids_return
{
    ids_action_t ids_action;
    uint8_t rule_number;
} ids_return_t;

ids_return_t check_packet(rule_t *rules, int number_rules, size_t buf_len, uint8_t *buf);
