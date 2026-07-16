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
#include <assert.h>
#include <poll.h>

#include "queue.h"
#include "../common/datapackets.h"
#include "util.h"
#include "game.h"

#define PORT 8080
#define BACKLOG_SIZE QUEUE_LEN / 2

int main(void) {
    int status = EXIT_SUCCESS;
    fprintf(stdout, "Starting battleships server...\n");
    fflush(stdout);

    // create a socketpair communication between processes
    int ipc[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, ipc) < 0) {
        fprintf(stderr, "Error creating socket pair for IPC.\n");
        perror("socket");
        return EXIT_FAILURE;
    }

    // fork this process to create a dedicated game process

    int server_un_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    int server_in_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_un_fd < 0 || server_in_fd < 0) {
        fprintf(stderr, "Error creating sockets.\n");
        perror("socket");
        return EXIT_FAILURE;
    }

    // IPv4 connections binding
    struct sockaddr_in addr_in;
    memset(&addr_in, 0, sizeof(struct sockaddr_in));
    addr_in.sin_addr.s_addr = INADDR_ANY;
    addr_in.sin_port = htons(PORT);
    addr_in.sin_family = AF_INET;

    // local connections binding
    struct sockaddr_un addr_un;
    memset(&addr_un, 0, sizeof(struct sockaddr_un));
    addr_un.sun_family = AF_UNIX;
    unlink("/tmp/battleships_socket");
    memcpy(addr_un.sun_path, "/tmp/battleships_socket", 24);

    queue players;
    q_init(&players);

    if (bind(server_un_fd, (struct sockaddr*) &addr_un, sizeof(struct sockaddr_un)) != 0 || bind(server_in_fd, (struct sockaddr*) &addr_in, sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "Error binding sockets.\n");
        perror("bind");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    if (listen(server_in_fd, BACKLOG_SIZE) < 0 || listen(server_un_fd, BACKLOG_SIZE) < 0) {
        fprintf(stderr, "Error listening on sockets.\n");
        perror("listen");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    datapacket packet;
    uint8_t buffer[PACKET_LEN];

    struct pollfd fds[3] = {
        {
            .fd = server_in_fd,
            .events = POLLIN
        }, 
        {
            .fd = server_un_fd,
            .events = POLLIN
        },
        {
            .fd = ipc[0],
            .events = POLLIN
        }
    };

    bool launched = false;
    for (int n = poll(fds, 2, -1); n >= 0; n = poll(fds, 2, -1)) {
        for (int i = 0; i < 2; ++i) {
            if (fds[i].revents & POLLIN) {
                int fd = accept(fds[i].fd, NULL, NULL);
                if (fd < 0) {
                    fprintf(stderr, "Error accepting connection.\n");
                    perror("accept");
                } else {
                    if (!launched)
                        new_player(&players, &packet, fd);
                    else if (!send_fd(fd))
                        fprintf(stderr, "Server parent-child IPC error.\n");
                        
                    if (!launched && q_size(&players) >= 2) {
                        launched = true;
                        if (!game_launch(ipc[1], (int[]) {ipc[0], server_in_fd, server_un_fd}, 3, &players)) {
                            status = EXIT_FAILURE;
                            goto cleanup;
                        }
                    } 
                }
            }
        }

        // game process sends message when game ends
        if (fds[2].revents & POLLIN) {
            uint8_t active[QUEUE_LEN + 1];
            ssize_t size = recv(ipc[0], active, sizeof(active), MSG_WAITALL);
            if (size != sizeof(active)) {
                fprintf(stderr, "Server parent-child IPC error.\n");
                perror("recv");
                status = EXIT_FAILURE;
                goto cleanup;
            }

            for (int i = 0; i < active[0]; ++i) {
                int player = -1;
                if (!q_pop(&players, &player)) {
                    fprintf(stderr, "Unknown server error regarding queue.\n");
                }
                
                // if players wish to play again they are added to the queue again
                if (active[i] > 0 && player != -1) {
                    new_player(&players, &packet, player);
                }
            }
            // TODO - restart queue
            // i.e. resend initial packet ? or do that at the end of the game?
        }


    }
    status = EXIT_FAILURE;
    perror("poll");
cleanup: 
    for (uint8_t i = players.front; i < players.back; ++i) {
        close(players.players[i % QUEUE_LEN]);
    }

    close(server_in_fd);
    close(server_un_fd);
    return status;
}