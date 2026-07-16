#include <stdbool.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

#include "game.h"
#include "../common/datapackets.h"
#include "util.h"

bool game_init(int server_fd, status* game_status, uint8_t* index, const char* nickname) {
    datapacket packet;
    uint8_t buffer[PACKET_LEN];

start:
    if (!read_packet(server_fd, &packet))
        return false;

    if (packet.type == full) {
        fprintf(stdout, "The server waiting queue is full. Please try to connect at another time.\n");
        return true;
    }

    if (packet.type == in_queue) {
        if (!game_spectate(server_fd, game_status, &packet))
            return false;

        fprintf(stdout, "Joining game...\n");
        if (packet.type != reinit)
            goto start;
    }

    if (packet.type != init) {
        fprintf(stderr, "Error in server-client syncronization.\n");
        return false;
    }

    *index = packet.data[0];
    char input[3];
    fprintf(stdout, "Start game? (Y/Q)\n");
    if (fgets(input, 3, stdin) == NULL) {
        fprintf(stderr, "Error while reading user-input.");
        return false;
    }

    packet.type = init;
    packet.data[0] = (uint8_t) (input[0] == 'Y');
    strcpy((char*) packet.data + 1, nickname);

    if (input[0] == 'Q') {
        fprintf(stdout, "Quitting game...\n");
        return write_packet(server_fd, &packet);
    }

    if (input[0] != 'Y') {
        fprintf(stderr, "Invalid user-input");
        write_packet(server_fd, &packet);
        return false;
    }

    strcpy(packet.data + 1, nickname);

    for (int i = 0; i < 4; ++i) {
        memset(game_status->boards[i].fields, '-', BOARD_SIZE * BOARD_SIZE);
    }
    
    if (!place_ships(&game_status->boards[*index]))
        return false;

    // packet.type = ships;
    memcpy(packet.data + 1 + NICKNAME_LEN, &game_status->boards[*index], sizeof(board));

    if (!write_packet(server_fd, &packet))
        return false;

    /*if (!read_packet(server_fd, &packet) || packet.type != public_ships)
        return false;

    memcpy(game_status, packet.data, sizeof(status));*/

    return game_loop(server_fd, game_status);
}

bool game_loop(int server_fd, status* game_status) {
    return false;
}

bool game_spectate(int server_fd, status* game_status, datapacket* packet) {
    if (!read_packet(server_fd, &packet))
        return false;

    if (packet->type == init)
        // means that some player didnt accept game and we moved on in the queue
        return true;

    if (packet->type != spectate) {
        fprintf(stderr, "Error in server-client syncronization.\n");
        return false;
    }
    
    fprintf(stdout, "Do you wish to spectate the ongoing game while waiting in the queue? (Y/N)\n");
    char input[3];
    if (fgets(input, 3, stdin) == NULL) {
        fprintf(stderr, "Error while reading user-input.");
        return false;
    }


    packet->type = spectate;
    packet->data[0] = (uint8_t) (input[0] == 'Y');
    if (!write_packet(server_fd, &packet))
        return false;

    if (input[0] == 'Y') {
        if (!game_spectate_loop(server_fd, game_status))
            return false;

        return true;
    }

    if (input[0] != 'N') {
        fprintf(stderr, "Invalid user-input.");    
        return false;
    }
    
    fprintf(stdout, "Waiting for the next game...\n");
    // will block until server sends another packet, for example at the start of the next game
    return read_packet(server_fd, packet, buffer);
}

bool game_spectate_loop();

bool game_start(int server_fd, const char* nickname) {
    status game_status;
    uint8_t index;
    return game_init(server_fd, &game_status, &index, nickname);
}