#pragma once

typedef void (*remove_handler_t)(void *);

typedef struct node
{
    void *user_data;
    struct node *previous;
    struct node *next;
} node_t;

typedef struct list {
    unsigned int list_size;
    node_t *first_node;
    node_t *last_node;
    remove_handler_t remove_handler;
} list_t;

/*
 * Adds a node to the specified list. The void * parameter corresponds 
 * to the user data. 
 */
void add_node(list_t *, void *);

/*
 * Removes the node from the specified list at the specified index.
 */
int remove_node(list_t *, unsigned int);

/*
 * Removes the first node from the list.
 */
int remove_first(list_t *);

/*
 * Removes the last node from the list.
 */
int remove_last(list_t *);

/*
 * Returns the user data stored in the list at the specified index, or NULL 
 * if the index is not correct.
 */
void* get(list_t *, int);