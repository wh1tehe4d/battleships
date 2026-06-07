#include <stdbool.h>

#include "../common/mechanics.h"

bool game_init(int server_fd);

bool game_loop(int server_fd);

bool game_start(int server_fd);

bool place_ship(ship ship);