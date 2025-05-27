#include "rank.h"
#include "network.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define SERVER_IP "192.168.0.1"
#define PORT 9000


int read_leaderboard(LeaderboardEntry *entries, char[] title, int max_entries) {
    connect_to_server(SERVER_IP, PORT);
    request_top10(title);
    disconnect();
}

void insert_rank(LeaderboardEntry entry) {
    connect_to_server(SERVER_IP, PORT);
    send_score(entry.nickname, entry.title, entry.score);
    disconnect();
}

void show_top_ranks(LeaderboardEntry *entries, int count) {
    
}