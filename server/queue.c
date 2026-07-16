#include <stdbool.h>

#include "queue.h"

bool q_init(queue* queue) {
    queue->front = 0;
    queue->back = 0;
    pthread_rwlock_init(&queue->lock, NULL);
    return true;
}

bool q_empty(queue* queue) {
    pthread_rwlock_rdlock(&queue->lock);
    bool result = queue->back == queue->front;
    pthread_rwlock_unlock(&queue->lock);
    return result;
}

bool q_full(queue* queue) {
    pthread_rwlock_rdlock(&queue->lock);
    bool result = queue->back == queue->front + QUEUE_LEN;
    pthread_rwlock_unlock(&queue->lock);
    return result;
}

bool q_append(queue* queue, int player) {
    pthread_rwlock_wrlock(&queue->lock);
    if (q_full(queue))
        return false;

    queue->players[queue->back % QUEUE_LEN] = player;
    queue->back++;
    pthread_rwlock_unlock(&queue->lock);
    return true;
}

bool q_pop(queue* queue, int* result) {
    pthread_rwlock_wrlock(&queue->lock);
    if (q_empty(queue))
        return false;

    *result = queue->players[queue->front % QUEUE_LEN];
    queue->front++;
    pthread_rwlock_unlock(&queue->lock);
    return true;
}

int q_get(queue* queue, uint8_t i) {
    pthread_rwlock_rdlock(&queue->lock);
    int result = queue->players[i % QUEUE_LEN];
    pthread_rwlock_unlock(&queue->lock);
    return result;
}

uint8_t q_size(queue* queue) {
    pthread_rwlock_rdlock(&queue->lock);
    uint8_t result = queue->back - queue->front;
    pthread_rwlock_unlock(&queue->lock);
    return result;
}
