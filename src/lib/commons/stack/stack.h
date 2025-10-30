/*
    This module contains the implementation of the stack
    that will be used in the project

    Here you can find all the functions you need to use
    and manipulate a stack
*/

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdlib.h>

// Forward declaration - opaque pointer
typedef void *Stack;

// Creation and destruction functions
Stack stack_create(void);
void stack_destroy(Stack stack);

// Stack manipulation functions
bool stack_push(Stack stack, void *data);
void *stack_pop(Stack stack);
void *stack_peek(Stack stack);
void *stack_peek_at(Stack stack, int index);

// Stack query functions
bool stack_is_empty(Stack stack);
int stack_size(Stack stack);

// Utility function
void stack_clear(Stack stack);

#endif // STACK_H
