#include "mechanics.h"

char get_symbol(ship ship) {
    switch (ship) {
        case carrier:
            return 'A';
        case battleship:
            return 'B';
        case cruiser:
            return 'C';
        case destroyer:
            return 'D';
        case submarine:
        default:
            return 'S';
    }
}

position get_pos(uint8_t* pos) {
    position p = {
                    pos[0] - '1',
                    pos[1] - 'A'
                };
    return p;
}