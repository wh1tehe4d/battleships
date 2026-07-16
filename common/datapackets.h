#ifndef DATA
#define DATA
#include <sys/types.h>
#include <netinet/in.h>
#include <stdint.h>

#include "mechanics.h"

#define PACKET_LEN sizeof(datapacket)
#define DATA_LEN sizeof(status)
#define IS_TURN(type) (type == turn_0 || type == turn_1 || type == turn_2 || type == turn_3)

typedef enum dp_type {
    turn_0 = 0, turn_1 = 1, turn_2 = 2, turn_3 = 3, init, reinit, ships, y_turn, n_turn, full, in_queue, spectate
} dp_type;

typedef struct datapacket {
    uint8_t type;
    uint8_t data[DATA_LEN];
} datapacket;

bool serialize_packet(datapacket* packet, uint8_t* buffer);

bool deserialize_packet(datapacket* packet, uint8_t* buffer);

bool read_packet(int fd, datapacket* packet/*, uint8_t* buffer*/);

bool write_packet(int fd, datapacket* packet/*, uint8_t* buffer*/);
#endif
