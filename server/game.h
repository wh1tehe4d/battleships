#ifndef GAME
#define GAME

#include <stdbool.h>
#include <poll.h>

#include "queue.h"
#include "../common/datapackets.h"

bool game(int parent_fd, queue* players);

bool game_launch(int parent_fd, int* others, int count, queue* players);

bool game_loop(status* game_status, queue* players, datapacket* packet, uint8_t* active, struct pollfd* fds);

int game_init(status* game_status, queue* players, datapacket* packet, uint8_t* active, struct pollfd* fds);


#endif