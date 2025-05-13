#include "leaderboard.h"
#include <time.h>


typedef struct LeaderboardEntry {
    char name[20];
    int score;
    time_t timestamp;

    struct LeaderboardEntry *prev;
    struct LeaderboardEntry *next;
} LeaderboardEntry;

LeaderboardEntry *leaderboard = NULL; // linked list of leaderboard entries

int read_leaderboard()
{
    // use open() and read() to read the leaderboard from a file
    // Update leaderboard array
    // return the number of entries read, or -1 on error

}

int write_leaderboard()
{
    // use open() and write() to write the leaderboard to a file
    // return 0 on success, -1 on error
    
}

int add_leaderboard_entry(const char *name, int score)
{
    // add a new entry to the leaderboard
    // return 0 on success, -1 on error
}

LeaderboardEntry topLeaderBoard[10];
LeaderboardEntry get_top10_leaderboard()
{
    // return the array of copied top 10 entries from the leaderboard
    // return NULL on error
    if (read_leaderboard() == -1) {
        return NULL;
    }

    return leaderboard;
}
