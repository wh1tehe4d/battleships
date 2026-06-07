#include <stdint.h>

#define BOARD_SIZE 10

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

position get_pos(uint8_t* pos);