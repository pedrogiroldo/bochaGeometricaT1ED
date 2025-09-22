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