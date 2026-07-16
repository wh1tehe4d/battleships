#ifndef MECH
#define MECH
#include <stdint.h>

#define BOARD_SIZE 10
#define NICKNAME_LEN 128
#define PLAYER_COUNT 4

typedef struct board {
    uint8_t fields[BOARD_SIZE][BOARD_SIZE];
} board;

typedef enum shot_result {
    hit, miss, sink
} shot_result;

typedef enum ship {
    carrier=5, battleship=4, cruiser=3, destroyer=2, submarine=1
} ship;

char get_symbol(ship ship);

typedef struct position {
    uint8_t row;
    uint8_t col;
} position;

typedef struct status {
    uint8_t player_count;
    uint8_t players[PLAYER_COUNT];
    uint8_t names[PLAYER_COUNT][NICKNAME_LEN];
    board boards[PLAYER_COUNT];
} status;
#endif