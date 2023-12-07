#include "cstack.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


struct node {
    struct node* prev;
    unsigned int data_size;
    void* p_data;
};

struct stack_head {
    hstack_t id;
    struct stack_head* next_head;
    unsigned int stack_size;
    struct node* p_node;
};


//This is the start-struct for the single-linked list
//of the structs that are beginnings of the stacks
struct stack_head head = {0, NULL, 0, NULL};



hstack_t stack_new(void)
{
    //variable for new stack-descriptors
    static hstack_t current_id = 0;

    //Allocate mem for the new stack head
    struct stack_head* temp_head = (struct stack_head*) malloc(sizeof(struct stack_head));

    // If malloc() returned NULL, so return -1
    if (temp_head == NULL)
        return -1;

    //Initialize all fields of the new head
    temp_head->id = ++current_id;
    temp_head->next_head = head.next_head;
    temp_head->stack_size = 0u;
    temp_head->p_node = NULL;


    //Link new structure with single-linked list of the heads
    head.next_head = temp_head;

    return current_id;
}


// If id is incorrect or stack with the id doesn't exist
// the function returns NULL
// otherwise it returns pointer to the stack_head with the id
struct stack_head* find_stack_head(hstack_t id) {

    //Incorrect id
    if (id <= 0)
        return NULL;

    struct stack_head* temp_head = &head;

    while (1) {

        if (temp_head->id == id) {
            return temp_head;
        }

        // It works because last head has "next_head = NULL"
        if (temp_head->next_head == NULL) {
            return NULL;
        }

        temp_head = temp_head->next_head;
    }
}

// Recursive function that allows to delete all nodes of the stack
// also it deletes all allocated data memory areas in nodes
void delete_nodes(struct node* current_node) {
    
    if (current_node == NULL) {
        return;
    }
    else {
        delete_nodes(current_node->prev);
        free(current_node->p_data);
        free(current_node);
    }
}


void stack_free(const hstack_t hstack)
{
    struct stack_head* prev_temp_head = &head;
    struct stack_head* temp_head = NULL;
    

    // If there are no stacks
    if (prev_temp_head->next_head == NULL)
        return;
    else
        temp_head = prev_temp_head->next_head;

    // Try to find needed stack
    bool stack_found = false;

    while (1) {

        if (temp_head->id == hstack) {
            stack_found = true;
            break;
        }

        if (temp_head->next_head != NULL) {
            prev_temp_head = temp_head;
            temp_head = temp_head->next_head;
        } else {
            break;
        }
    }

    // Delete all nodes if the stack was found
    if (stack_found) {
        delete_nodes(temp_head->p_node);
    } else {
        return;
    }

    // Delete the head of the stack itself
    // and link the both parts of the single-linked list together
    prev_temp_head->next_head = temp_head->next_head;
    free(temp_head);
}


int stack_valid_handler(const hstack_t hstack)
{
    if (find_stack_head(hstack) != NULL)
        return 0;
    else
        return 1;
}


unsigned int stack_size(const hstack_t hstack)
{
    struct stack_head* temp_head;
    if ( (temp_head = find_stack_head(hstack)) != NULL)
        return temp_head->stack_size;
    else
        return 0u;
}


void stack_push(const hstack_t hstack, const void* data_in, const unsigned int size)
{
    // Check if arguments are valid
    if (data_in == NULL)
        return;

    if (size == 0u)
        return;

    struct stack_head* temp_head = find_stack_head(hstack);
    if (temp_head == NULL)
        return;


    // Create new node for new data
    struct node* new_node = malloc(sizeof(struct node));
    if (new_node == NULL)
        return;

    //Try to allocate memory for the new data in new node
    new_node->p_data = malloc(size);
    if (new_node->p_data == NULL) {
        free(new_node);
        return;
    }

    //Allocate other fields of the new node
    new_node->data_size = size;
    new_node->prev = temp_head->p_node;
    
    //Copy data
    memcpy(new_node->p_data, data_in, size);
    
    //Link new node with the stack
    temp_head->p_node = new_node;
    ++(temp_head->stack_size);
}


unsigned int stack_pop(const hstack_t hstack, void* data_out, const unsigned int size)
{
    // Check if arguments are valid
    if (data_out == NULL)
        return 0u;

    if (size == 0u)
        return 0u;

    struct stack_head* temp_head = find_stack_head(hstack);
    if (temp_head == NULL)
        return 0u;

    //Fetch needed node
    struct node* upper_node = temp_head->p_node;
    if (upper_node == NULL)
        return 0u;

    // If the buffer's capacity is less than the data-size in the node
    if (size < upper_node->data_size)
        return 0u;

    //Copy the data from node to the outer buffer
    memcpy(data_out, upper_node->p_data, upper_node->data_size);
    
    //Link previous node with the stack-head-structure
    //making the node the upper one
    temp_head->p_node = upper_node->prev;
    --(temp_head->stack_size);


    //Delete the upper_node
    unsigned int copied_size = upper_node->data_size;
    free(upper_node->p_data);
    free(upper_node);

    return copied_size;
}

