#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <memory.h>

#include "datapackets.h"

bool serialize_packet(datapacket* packet, uint8_t* buffer) {
    buffer[0] = packet->type;
    memcpy(buffer + 1, packet->data, DATA_LEN);
    return true;
}

bool deserialize_packet(datapacket* packet, uint8_t* buffer) {
    packet->type = buffer[0];
    memcpy(packet->data, buffer + 1, DATA_LEN);
    return true;
}

bool read_packet(int server_fd, datapacket* packet/*, uint8_t* buffer*/) {
    ssize_t size = recv(server_fd, (void *) packet, PACKET_LEN, MSG_WAITALL);
    if (size != PACKET_LEN/* || !deserialize_packet(packet, buffer)*/) {
        fprintf(stderr, "Client-Server communication error.\n");
        return false;
    }

    return true;
}

bool write_packet(int fd, datapacket* packet/*, uint8_t* buffer*/) {
    /*if (!serialize_packet(packet, buffer)) {
        fprintf(stderr, "Client-Server communication error.\n");
        return false;
    }*/

    ssize_t size = send(fd, (void *) packet, PACKET_LEN, MSG_WAITALL);
    if (size != PACKET_LEN) {
        fprintf(stderr, "Client-Server communication error.\n");
        return false;
    }

    return true;
}