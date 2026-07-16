#ifndef QUEUE
#define QUEUE
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

#define QUEUE_LEN 64

typedef struct queue {
    uint8_t front;
    uint8_t back;
    int players[QUEUE_LEN];
    pthread_rwlock_t lock;
} queue;

bool q_init(queue* queue);

bool q_append(queue* queue, int player);

bool q_pop(queue* queue, int* result);

bool q_empty(queue* queue);

bool q_full(queue* queue);

uint8_t q_size(queue* queue);
#endif