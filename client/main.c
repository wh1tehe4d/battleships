#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/un.h>
#include <stdlib.h>

#include "../common/datapackets.h"
#include "game.h"
#include "util.h"

int main(int argc, void** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    } else if (argc != 3) {
        fprintf(stderr, "Invalid argument count. For usage use --help.\n");
        return EXIT_FAILURE;
    }

    if (strlen(argv[2]) > NICKNAME_LEN) {
        fprintf(stderr, "Nickname is too long. Maximum allowed length is 127 characters.\n");
        return EXIT_FAILURE;
    }

    size_t ip_len = strlen(argv[1]);
    if (ip_len > 20 || ip_len < 8) {
        fprintf(stderr, "Invalid IPv4 adress supplied.\n");
        return EXIT_FAILURE;
    }
    
    const char* ip = strtok(argv[1], ":");
    const char* port = strtok(NULL, ":");
    const char* nickname = argv[2];

    
    // TODO
    // find out if the server is local or internet

    bool local = true;
    int server_fd = socket(local ? AF_UNIX : AF_INET6, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fprintf(stderr, "Error creating socket.\n");
        return EXIT_FAILURE;
    }
    
    if (local) {
        struct sockaddr_un addr;

        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        memcpy(addr.sun_path, "/tmp/battleships_socket", 24);

        if (connect(server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
            perror("connect");
            fprintf(stderr, "Error connecting to the local game server.\n");
            return EXIT_FAILURE;
        }
    } else {
        struct sockaddr_in addr;

        memset(&addr, 0, sizeof(struct sockaddr_in));
        addr.sin_family = AF_INET;
        if (inet_aton(ip, &addr.sin_addr) != 0) {
            fprintf(stderr, "Invalid IPv4 address format.\n");
            return EXIT_FAILURE;
        }

        addr.sin_port = htons((uint16_t) atoi(port));

        if (connect(server_fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
            fprintf(stderr, "Error connecting to game server.\n");
            return EXIT_FAILURE;
        }
    }

    int return_val = game_start(server_fd, nickname) ? EXIT_SUCCESS : EXIT_FAILURE;
    return_val = close(server_fd) == 0 ? return_val : EXIT_FAILURE;
    return return_val;
}