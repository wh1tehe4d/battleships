#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#include "util.h"

bool get_position(const char letter, const char* number, position dimensions, position* output) {
    int row = atoi(number) - 1;
    int col = letter - 'A';
    if (row < 0 || (uint8_t) row > dimensions.row || col < 0 || (uint8_t) col > dimensions.col)
        return false;
    
    output->row = (uint8_t) row;
    output->col = (uint8_t) col;
    return true;
}

bool place_ship(board* board, ship ship, position pos, bool horizontal) {
    for (int i = 0; i < (int) ship; ++i) {
        if (board->fields[pos.row][pos.col] != (uint8_t) '-')
            return false;
        
        board->fields[pos.row][pos.col] = get_symbol(ship);
        if (horizontal)
            pos.col++;
        else
            pos.row++;
    }

    return true;
}

bool place_ships(board* board) {
    const char* ship_names[] = {"Aircraft Carrier", "Battleship", "Cruiser", "Destroyer", "Destroyer", "Submarine", "Submarine"};
    const ship ship_lengths[] = {carrier, battleship, cruiser, destroyer, destroyer, submarine, submarine};

    char input[INPUT_LEN];
    for (int i = 0; i < 7; ++i) {
    input_ship:
        fprintf(stdout, "Please enter the starting cell for the %s:\n", ship_names[i]);
        if (fgets(input, INPUT_LEN, stdin) == NULL) {
            fprintf(stderr, "Error while reading user-input.");
            return false;
        }
        position position;
        if (!get_position(input[0], input + 1, dimensions, &position)) {
            fprintf(stderr, "Invalid user input - dimensions.\n");
            goto input_ship;
        }

        fprintf(stdout, "Please enter the orientation (H/V):\n");
        if (fgets(input, 3, stdin) == NULL) {
            fprintf(stderr, "Error while reading user-input.");
            return false;
        }
        if (input[0] != 'H' && input[0] != 'V') {
            fprintf(stderr, "Invalid user input.\n");
            goto input_ship;
        }

        if (!place_ship(board, ship_lengths[i], position, input[0] == 'H')) {
            fprintf(stderr, "You can't place boats on top of other boats.\n");
            goto input_ship;
        }
    }

    return true;
}

void print_board(board* board) {

}

void print_help(const char* file_name) {
    printf("Battleships client interface.\n");
    printf("Run using:\n");
    printf("%s <IP:PORT of the server> <nickname>\n", file_name);
}