#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>

#include "datapackets.h"

bool serialize_packet(datapacket* packet, uint8_t* buffer) {
    buffer[0] = packet->type;
    uint8_t* other = (uint8_t* ) memcpy(buffer + 1, packet->data, DATA_LEN);
    return other == buffer + DATA_LEN;
}

bool deserialize_packet(datapacket* packet, uint8_t* buffer) {
    packet->type = buffer[0];
    uint8_t* other = (uint8_t* ) memcpy(packet->data, buffer + 1, DATA_LEN);
    return other == packet->data + DATA_LEN;
}

bool read_packet(int server_fd, datapacket* packet, dp_type expected, uint8_t* buffer) {
    ssize_t size = read(server_fd, (void *) buffer, DATA_LEN + 1);
    if (size != sizeof(datapacket) || !deserialize_packet(packet, buffer) || packet->type != expected) {
        fprintf(stderr, "Client-Server communication error.");
        return false;
    }
}

bool write_packet(int fd, datapacket* packet, uint8_t* buffer) {
    if (!serialize_packet(packet, buffer)) {
        fprintf(stderr, "Client-Server communication error.");
        return false;
    }

    ssize_t size = write(fd, (void *) buffer, DATA_LEN + 1);
    if (size != sizeof(datapacket) || !deserialize_packet(packet, buffer)) {
        fprintf(stderr, "Client-Server communication error.");
        return false;
    }
}