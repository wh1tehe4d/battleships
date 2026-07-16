#!/usr/bin/bash
gcc client/main.c client/util.c client/game.c common/datapackets.c common/mechanics.c -o client_app
gcc server/main.c server/util.c server/queue.c server/game.c common/datapackets.c common/mechanics.c -o server_app