#ifndef UTIL
#define UTIL
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>

#include "../common/mechanics.h"

#define INPUT_LEN ((int) floor(log10(BOARD_SIZE)) + 4)

static const position dimensions = {.row = BOARD_SIZE, .col = BOARD_SIZE};

bool get_position(const char letter, const char* number, position dimensions, position* output);

bool place_ship(board* board, ship ship, position pos, bool horizontal);

bool place_ships(board* board);

void print_board(board* board);

void print_help(const char* file_name);
#endif
