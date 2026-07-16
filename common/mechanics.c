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