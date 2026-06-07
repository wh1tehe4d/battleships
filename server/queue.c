#include <stdbool.h>

#include "queue.h"

bool q_init(queue queue) {
    queue->front = 0;
    queue->back = 0;
    return true;
}

bool q_append(queue queue, int player) {
    if (queue->back = queue->front + QUEUE_LEN)
        return false;

    queue->players[queue->back % QUEUE_LEN] = player;
    queue->back++;
    return true;
}

int q_pop(queue queue) {
    if (queue->back == queue->front)
        return -1;

    int player = queue->players[queue->front % QUEUE_LEN];
    queue->front++;
    return player;
}