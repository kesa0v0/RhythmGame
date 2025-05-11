#include "rank.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int compare_scores(const void *a, const void *b) {
    return ((LeaderboardEntry *)b)->score - ((LeaderboardEntry *)a)->score;
}

int read_leaderboard(LeaderboardEntry *entries, int max_entries) {
    int fd = open(RANK_FILE, O_RDONLY);
    if (fd < 0) return 0;
    int count = read(fd, entries, sizeof(LeaderboardEntry) * max_entries) / sizeof(LeaderboardEntry);
    close(fd);
    return count;
}

int write_leaderboard(LeaderboardEntry *entries, int count) {
    int fd = open(RANK_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    write(fd, entries, sizeof(LeaderboardEntry) * count);
    close(fd);
    return 0;
}

void insert_rank(LeaderboardEntry *entries, int *count, const char *name, int score) {
    if (*count < MAX_RANKS) {
        strncpy(entries[*count].name, name, sizeof(entries[*count].name) - 1);
        entries[*count].score = score;
        entries[*count].timestamp = time(NULL);
        (*count)++;
    } else if (score > entries[*count - 1].score) {
        strncpy(entries[*count - 1].name, name, sizeof(entries[*count - 1].name) - 1);
        entries[*count - 1].score = score;
        entries[*count - 1].timestamp = time(NULL);
    }
    qsort(entries, *count, sizeof(LeaderboardEntry), compare_scores);
    if (*count > MAX_RANKS) *count = MAX_RANKS;
}

void show_top_ranks(LeaderboardEntry *entries, int count) {
    printf("\n===== TOP %d RANKS =====\n", count);
    if(count == 0){
        printf("No data\n");
    }
    for (int i = 0; i < count; i++) {
        printf("%2d. %-20s %5d\n", i + 1, entries[i].name, entries[i].score);
    }
    printf("=======================\n\n");
}