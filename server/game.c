#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <poll.h>
#include <math.h>
#include <time.h>
#include <memory.h>
#include <pthread.h>

#include "game.h"
#include "queue.h"
#include "../common/mechanics.h"
#include "util.h"

#define TIMEOUT 60000

bool game(int parent_fd, queue* players) {
    status game_status;
    memset(&game_status, 0, sizeof(status));

    struct pollfd fds[PLAYER_COUNT + 1];
    fds[0] = (struct pollfd) {
        .fd = parent_fd,
        .events = POLLIN
    };

    game_status.player_count = MIN(PLAYER_COUNT, q_size(players));
    for (uint8_t i = players->front; i < game_status.player_count; ++i) {
        fds[(i % QUEUE_LEN) + 1] = (struct pollfd) {
            .fd = players->players[i % QUEUE_LEN],
            .events = POLLIN | POLLERR | POLLHUP
        };
    }

    datapacket packets[2];

    uint8_t active[QUEUE_LEN + 1];
    active[0] = q_size(players);
    memset(active + 1, 1, active[0]);
    
    int player_count = game_init(&game_status, players, packets, active, fds);
    if (player_count < 0)
        return false;
    else if (player_count == 0)
        return true;

    game_status.player_count = (uint8_t) player_count;
    
    
    

    
    return true;
}

typedef struct spectator_manager {
    int parent;
    queue* players;
    uint8_t* inactive_players;
    pthread_rwlock_t lock
} spectator_manager;

bool game_spectator_manager(spectator_manager* manager) {
    int fd;
    for (bool result = recv_fd(&fd); )
}

typedef struct send_info {
    int fd;
    datapacket* packet
} send_info;

bool game_status_send(send_info* info) {
    return write_packet(info->fd, info->packet);
}

bool game_turn(status* game_status, datapacket** packets, uint8_t whos_turn) {
    for (int i = 0; i < 2; ++i) {
        memcpy(packets[i]->data, &game_status, sizeof(status));
    }
    packets[0]->type = whos_turn;

    send_info info[4];
    pthread_t threads[4];
    for (uint8_t i = 0; i < game_status->player_count; ++i) {
        info[i].fd = game_status->players[i];
        info[i].packet = packets[0];
        pthread_create(&threads[i], NULL, &game_status_send, &info);
    }

    for (uint8_t i = 0; i < game_status->player_count; ++i) {
        bool success = true;
        pthread_join(threads[i], &success);
    }

    if (!read_packet(game_status->players[whos_turn], packets[0]))
        return false;

    memcpy(game_status, packets[0]->data, sizeof(status));

}

int game_init(status* game_status, queue* players, datapacket* packet, uint8_t* active, struct pollfd* fds) {
    int j = 0;
    for (; j < game_status->player_count;) {
        int n = poll(fds, 5, TIMEOUT);
        if (n < 0) {
            fprintf(stderr, "FATAL Error getting info from clients.\n");
            perror("poll");
            return -1;
        } else if (n == 0 && j >= 2) {
            for (int i = 0; i < game_status->player_count; ++i) {
                if (game_status->players[i] == 0) {
                    // remove fd from next poll
                    close(fds[i + 1].fd);
                    fds[i + 1].fd = next_player(players, game_status->player_count, i - 1, packet, active);
                    active[i + 1] = 0;
                }
            }
            break;
        } else if (n == 0) {
            fprintf(stderr, "Players didn't respond in time.\n");
            return 0;
        }

        if (fds[0].revents & POLLIN) {
            int fd;
            if (recv_fd(&fd) && new_player(players, packet, fd)) {
                active[0]++;
                if (game_status->player_count < 4) {
                    fds[game_status->player_count + 1] = (struct pollfd) {
                        .fd = fd,
                        .events = POLLIN | POLLERR | POLLHUP
                    };
                }
                game_status->player_count++;
            }
        }

        for (int i = 1; i < game_status->player_count + 1; ++i) {
            if (fds[i].revents & POLLIN) {
                if (!read_packet(fds[i].fd, packet)) {
                    close(fds[i].fd);
                    fds[i].fd = next_player(players, game_status->player_count, i - 1, packet, active);
                    active[i] = 0;
                }

                if (packet->type != init || game_status->players[i - 1] != 0) {
                    fprintf(stderr, "Syncronization error with player %d. Removing him from the game.\n", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = next_player(players, game_status->player_count, i - 1, packet, active);
                    active[i] = 0;
                    game_status->players[i - 1] = 0;
                }

                if (packet->data[0] == 0) {
                    fprintf(stderr, "Player %d didn't confirm joining the game. Trying next player in queue\n.", fds[i].fd);
                    close(fds[i].fd);
                    fds[i].fd = next_player(players, game_status->player_count, i - 1, packet, active);
                    active[i] = 0;
                }

                strcpy(game_status->names + i - 1, packet->data + 1);
                memcpy(game_status->boards + i - 1, packet->data + 1 + NICKNAME_LEN, sizeof(board));
                game_status->players[i - 1] = fds[i].fd;
                --j;

            } else if (fds[i].revents & (POLLHUP | POLLERR)) {
                close(fds[i].fd);
                fds[i].fd = -1;
                active[i] = 0;
            }
        }
    }

    return j;
}

bool game_launch(int parent_fd, int* others, int count, queue* players) {
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "FATAL Error creating server child process.\n");
        perror("fork");
        return false;
    } else if (pid > 0) {
        close(parent_fd);
        return true;
    }

    for (int i = 0; i < count; ++i)
        close(others[i]);
    
    int status = game(parent_fd, players) ? EXIT_SUCCESS : EXIT_FAILURE;

    exit(status);
}

bool game_loop(status* game_status, queue* players, datapacket* packet, uint8_t* active, struct pollfd* fds) {}