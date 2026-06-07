#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>

#include "../common/datapackets.h"
#include "game.h"

void print_help(const char* file_name) {
    printf("Battleships client interface.\n");
    printf("Run using:\n");
    printf("%s <IP:PORT of the server> <nickname>", file_name);
}

int main(int argc, void** argv) {
    if (argc == 2 && strcmp(argv[1], "--help")) {
        print_help(argv[0]);
        return 0;
    } else if (argc != 3) {
        fprintf(stderr, "Invalid argument count. For usage use --help.");
        return 1;
    }

    const char* ip = argv[1];
    const char* nickname = argv[2];

    char host[256];
    if (gethostname(host, 256) != 0) {
        fprintf(stderr, "Error getting hostname.");
        return 1;
    }

    struct hostent* entry;
    entry = gethostbyname(host);
    bool local = false;
    for (int i = 0; entry->h_addr_list[i] != NULL; ++i) {
        char* adress = entry->h_addr_list[i];
        if (memcmp(ip, adress, entry->h_length) == 0) {
            local = true;
            break;
        }
    }

    int server_fd = socket(local ? AF_UNIX : AF_INET6, SOCK_STREAM, 0);;
    if (server_fd == -1) {
        fprintf(stderr, "Error creating socket.");
        return 1;
    }

    return game_start(server_fd);
}