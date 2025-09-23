/*
    This module contains the implementation of the queue
    that will be used in the project

    Here you can find all the functions you need to use
    and manipulate a queue
*/
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdlib.h>

// Forward declaration - opaque pointer
typedef struct Queue Queue;

// Creation and destruction functions
Queue *queue_create(void);
void queue_destroy(Queue *queue);

// Queue manipulation functions
bool queue_enqueue(Queue *queue, void *data);
void *queue_dequeue(Queue *queue);
void *queue_peek(Queue *queue);

// Queue query functions
bool queue_is_empty(Queue *queue);
int queue_size(Queue *queue);

// Utility function
void queue_clear(Queue *queue);

#endif // QUEUE_H