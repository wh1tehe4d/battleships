#include <stdbool.h>

#define QUEUE_LEN 64

struct queue_t {
    int front;
    int back;
    int players[QUEUE_LEN];
};

typedef struct queue_t* queue;

bool q_init(queue queue);

bool q_append(queue queue, int player);

int q_pop(queue queue);