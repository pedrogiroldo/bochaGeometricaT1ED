#include "queue.h"
#include <stdio.h>

// Internal structure definitions - only visible in implementation
typedef struct QueueNode {
  void *data;
  struct QueueNode *next;
} QueueNode;

// Complete Queue structure definition
struct Queue {
  QueueNode *front; // First element in queue
  QueueNode *rear;  // Last element in queue
  int size;         // Current queue size
};

/**
 * Creates a new empty queue
 * @return Pointer to new queue or NULL on error
 */
Queue *queue_create(void) {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  if (queue == NULL) {
    return NULL;
  }

  queue->front = NULL;
  queue->rear = NULL;
  queue->size = 0;

  return queue;
}

/**
 * Destroys the queue and frees all associated memory
 * @param queue Pointer to queue to be destroyed
 */
void queue_destroy(Queue *queue) {
  if (queue == NULL) {
    return;
  }

  queue_clear(queue);
  free(queue);
}

/**
 * Adds an element to the end of the queue
 * @param queue Pointer to the queue
 * @param data Data to be added
 * @return true on success, false on error
 */
bool queue_enqueue(Queue *queue, void *data) {
  if (queue == NULL) {
    return false;
  }

  QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
  if (new_node == NULL) {
    return false;
  }

  new_node->data = data;
  new_node->next = NULL;

  if (queue_is_empty(queue)) {
    queue->front = new_node;
    queue->rear = new_node;
  } else {
    queue->rear->next = new_node;
    queue->rear = new_node;
  }

  queue->size++;
  return true;
}

/**
 * Removes and returns the first element from the queue
 * @param queue Pointer to the queue
 * @return Data from first element or NULL if queue is empty
 */
void *queue_dequeue(Queue *queue) {
  if (queue == NULL || queue_is_empty(queue)) {
    return NULL;
  }

  QueueNode *node_to_remove = queue->front;
  void *data = node_to_remove->data;

  queue->front = queue->front->next;

  // If queue became empty, update rear as well
  if (queue->front == NULL) {
    queue->rear = NULL;
  }

  free(node_to_remove);
  queue->size--;

  return data;
}

/**
 * Returns the first element from the queue without removing it
 * @param queue Pointer to the queue
 * @return Data from first element or NULL if queue is empty
 */
void *queue_peek(Queue *queue) {
  if (queue == NULL || queue_is_empty(queue)) {
    return NULL;
  }

  return queue->front->data;
}

/**
 * Checks if the queue is empty
 * @param queue Pointer to the queue
 * @return true if empty, false otherwise
 */
bool queue_is_empty(Queue *queue) {
  return (queue == NULL || queue->front == NULL);
}

/**
 * Returns the current size of the queue
 * @param queue Pointer to the queue
 * @return Number of elements in the queue
 */
int queue_size(Queue *queue) {
  if (queue == NULL) {
    return 0;
  }

  return queue->size;
}

/**
 * Removes all elements from the queue
 * @param queue Pointer to the queue
 */
void queue_clear(Queue *queue) {
  if (queue == NULL) {
    return;
  }

  while (!queue_is_empty(queue)) {
    queue_dequeue(queue);
  }
}