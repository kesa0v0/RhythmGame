#include "rank.h"
#include "network.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define SERVER_IP "127.0.0.1"
#define PORT 9000


int read_leaderboard(LeaderboardEntry *entries, char *title, int max_entries) {
    connect_to_server(SERVER_IP, PORT);
    // get top 10 entries and save them in entries
    if (!request_top10(title, entries)) {
        disconnect();
        return 0; // Failed to get leaderboard
    }
    disconnect();
    return 1;
}

void insert_rank(char *nickname, char *title, int score) {
    connect_to_server(SERVER_IP, PORT);
    send_score(nickname, title, score);
    disconnect();
}

void show_top_ranks(LeaderboardEntry *entries, int count) {
    printf("Top %d Ranks:\n", count);
    for (int i = 0; i < count; i++) {
        printf("%d. %s - %s: %d\n", i + 1, entries[i].nickname, entries[i].title, entries[i].score);
    }
}