#ifndef RANK_H
#define RANK_H

#include "network.h"
#include <time.h>

#define MAX_RANKS 10

int read_leaderboard(LeaderboardEntry *entries, char *title, int max_entries);
void insert_rank(char *nickname, char *title, int score);
void show_top_ranks(LeaderboardEntry *entries, int count);

#endif // RANK_H