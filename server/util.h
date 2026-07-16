#ifndef UTIL
#define UTIL

#include "queue.h"
#include "../common/datapackets.h"

#define MIN(a, b) (a < b ? a : b)

bool new_player(queue* players, datapacket* datapacket, int socket);

bool recv_fd(int* fd);

bool send_fd(int fd);

bool remove_players(queue* queue, uint8_t* active, uint8_t count);

int next_player(queue* queue, uint8_t player_count, uint8_t index, datapacket* packet, uint8_t* active);

#endif