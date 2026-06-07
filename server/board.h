#include <stdio.h>
#include <stdbool.h>

#include "../common/datapackets.h"
#include "../common/mechanics.h"



void print_board(board board);

void place_ship(char type, position pos, bool horizontal);

shot_result shot(board board, position pos);