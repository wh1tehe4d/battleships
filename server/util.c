#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"
#include "../common/datapackets.h"

bool new_player(queue* players, datapacket* packet, int socket) {
    bool result = true;
    if (!q_append(players, socket)) {
        packet->type = full;
        result = false;
        close(socket);
    } else if (q_size(players) > 4) {
        packet->type = in_queue;
        packet->data[0] = q_size(players);
    } else {
        packet->type = init;
        packet->data[0] = q_size(players);
    }

    if (!write_packet(socket, packet)) {
        fprintf(stderr, "Player %d wasn't initialized correctly.\n", socket);
        assert(q_size(players) > 0);
        players->back--;
        result = false;
        close(socket);
    }

    return result;
}

bool remove_players(queue* queue, uint8_t* active, uint8_t count) {}

int next_player(queue* queue, uint8_t player_count, uint8_t index, datapacket* packet, uint8_t* active) {
    if (q_size(queue) == player_count)
        return -1;

    packet->type = reinit;
    packet->data[0] = index;

    uint8_t i = (queue->front + player_count) % QUEUE_LEN;
    int fd = queue->players[i];
    
    if (write_packet(fd, packet)) 
        return fd;
    
    active[i + 1] = 0;
    // tail recursion in C, not ideal but OK since we dont expect more than 64 players in queue
    return next_player(queue, player_count + 1, index, packet, active); 
}

bool recv_fd(int* fd) {
    return false;
}

bool send_fd(int fd) {
    return false;
}