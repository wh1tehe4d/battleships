#ifndef GAME
#define GAME
#include <stdbool.h>

#include "../common/mechanics.h"

bool game_init(int server_fd, status* game_status, uint8_t* index, const char* nickname);

bool game_loop(int server_fd, status* game_status);

bool game_start(int server_fd, const char* nickname);

bool game_spectate(int server_fd, status* game_status);
#endif