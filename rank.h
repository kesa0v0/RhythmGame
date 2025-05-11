#ifndef RANK_H
#define RANK_H

#include <time.h>

#define MAX_RANKS 10
#define RANK_FILE "rank.txt"

typedef struct {
    char name[20];
    int score;
    time_t timestamp;
} LeaderboardEntry;

int read_leaderboard(LeaderboardEntry *entries, int max_entries);
int write_leaderboard(LeaderboardEntry *entries, int count);
void insert_rank(LeaderboardEntry *entries, int *count, const char *name, int score);
void show_top_ranks(LeaderboardEntry *entries, int count);

#endif // RANK_H