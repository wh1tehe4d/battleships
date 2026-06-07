#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>

#include "game.h"
#include "../common/datapackets.h"

bool place_ship(board* board, ship ship, position pos, bool horizontal) {

}

bool game_init(int server_fd) {
    datapacket packet;
    uint8_t buffer[DATA_LEN + 1];
    if (!read_packet(server_fd, &packet, init, buffer))
        return false;

    packet.type = init;
    packet.data[0] = 1;

    if (!write_packet(server_fd, &packet, buffer))
        return false;


    return true;
}

bool game_loop(int server_fd) {
    return false;
}

bool game_start(int server_fd) {
    if (!game_init(server_fd) || !game_loop(server_fd))
        return false;
}