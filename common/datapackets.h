#include <sys/types.h>
#include <netinet/in.h>
#include <stdint.h>

#include "mechanics.h"

#define DATA_LEN 1024

typedef enum dp_type {
    init, ships, turn, hit, miss, sink
} dp_type;

typedef struct datapacket {
    dp_type type;
    uint8_t data[DATA_LEN];
} datapacket;

bool serialize_packet(datapacket* packet, uint8_t* buffer);

bool deserialize_packet(datapacket* packet, uint8_t* buffer);

bool read_packet(int fd, datapacket* packet, dp_type expected, uint8_t* buffer);

bool write_packet(int fd, datapacket* packet, uint8_t* buffer);
