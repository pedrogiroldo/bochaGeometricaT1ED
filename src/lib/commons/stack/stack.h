#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stdlib.h>

// Forward declaration - opaque pointer
typedef struct Stack Stack;

// Creation and destruction functions
Stack *stack_create(void);
void stack_destroy(Stack *stack);

// Stack manipulation functions
bool stack_push(Stack *stack, void *data);
void *stack_pop(Stack *stack);
void *stack_peek(Stack *stack);

// Stack query functions
bool stack_is_empty(Stack *stack);
int stack_size(Stack *stack);

// Utility function
void stack_clear(Stack *stack);

#endif // STACK_H
