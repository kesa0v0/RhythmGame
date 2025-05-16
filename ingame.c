#include "audio.h"
#include "rank.h"

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#define HEIGHT 20
#define WIDTH  20

#define TIMING_LINE ((HEIGHT) - 4)
#define SCORE_LINE (HEIGHT)
#define HIT_LINE ((HEIGHT) - 1)

#define NUM_LANES 4
#define LANE_WIDTH (WIDTH / NUM_LANES)


int score = 0;

char username[20];
LeaderboardEntry ranks[MAX_RANKS];

bool is_game_paused = false;


#pragma region Note

#define NOTE_CHAR 'A'

typedef struct {
    int lane;
    int y;
    int active;
} Note;

#define MAX_NOTES 100
Note notes[MAX_NOTES];
int note_start = 0;
int note_end = 0;
int note_count = 0;

// Note 구조체 연결리스트
typedef struct BeatMapNote {
    int hit_ms;
    int lane;
    struct BeatMapNote* next;
} BeatMapNote;
BeatMapNote* beatmap = NULL;
char song_name[256];
char song_path[1024];
int song_length = 0;
int bpm = 0;

void read_beatmap(const char* filename) {
    // read() 시스템콜로 beatmap 파일을 읽고 Note 구조체의 연결리스트로 저장
    char buffer[1024];

    int fd = open(filename, O_RDONLY);


    //TODO: 읽고 연결리스트로 변환하는 코드 작성

    return 0;
}

void spawn_note(int lane) {
    if (note_count < MAX_NOTES) {
        notes[note_end].lane = lane;
        notes[note_end].y = 0;
        notes[note_end].active = 1;
        note_end = (note_end + 1) % MAX_NOTES;
        note_count++;
    }
}

void update_notes() {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (notes[i].active) {
            notes[i].y++;
            if (notes[i].y > HEIGHT) {
                notes[i].active = 0;
            }
        }
    }
}

void draw_notes() {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (notes[i].active) {
            int x = notes[i].lane * LANE_WIDTH + LANE_WIDTH / 2;
            mvaddch(notes[i].y, x, NOTE_CHAR);
        }
    }
}

#pragma endregion


void handle_input(int ch) {
    int lane = -1;
    if (ch == 'q') lane = 0;
    else if (ch == 'w') lane = 1;
    else if (ch == 'e') lane = 2;
    else if (ch == 'r') lane = 3;

    if (lane != -1) {
        int judged = 0;
        bool isNoteStartBiggerThanEnd = note_start > note_end;
        int modEnd = isNoteStartBiggerThanEnd ? note_end + MAX_NOTES : note_end;
        for (int i = note_start; i < modEnd; i++) {
            if (notes[i % MAX_NOTES].active && notes[i % MAX_NOTES].lane == lane) {
                int diff = notes[i % MAX_NOTES].y - TIMING_LINE;
                printf("note: %d, diff: %d\n", notes[i % MAX_NOTES].y, diff);
                if (diff >= -1 && diff <= 1) {
                    mvprintw(HEIGHT, 0, "Perfect!                    ");
                    notes[i % MAX_NOTES].active = 0;
                    judged = 1;
                    score += 1;
                    break;
                }
            }
        }
        if (!judged) {
            mvprintw(HEIGHT, 0, "Miss...                      ");
        }

        refresh();
    }
}

void close_program() {
    audio_close();
    endwin();
    exit(0);
}

void init_ncurses() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}

void pause_game() {
    char answer;
    int count = 0;

    is_game_paused = true;
    audio_pause_bgm();

    endwin();
    printf("\n=== GAME PAUSED ===\n");

    count = read_leaderboard(ranks, MAX_RANKS);
    show_top_ranks(ranks, (count > 3 ? 3 : count));

    while (1) {
        printf("Continue game? (y/n): ");
        scanf(" %c", &answer);
        if (answer == 'y' || answer == 'Y') {
            break;
        } else if (answer == 'n' || answer == 'N') {
            printf("Exiting game without saving data...\n");
            exit(0);
        } else {
            printf("Invalid input. Please enter 'y' or 'n'.\n");
        }
    }

    init_ncurses();
    audio_resume_bgm();
    is_game_paused = false;
}

void handle_game_over(const char *username, int score) {
    int count = read_leaderboard(ranks, MAX_RANKS);
    insert_rank(ranks, &count, username, score);
    write_leaderboard(ranks, count);

    endwin();
    printf("\n===== GAME OVER =====\n");
    show_top_ranks(ranks, (count > 10 ? 10 : count));

    for (int i = 0; i < count; i++) {
        if (strcmp(ranks[i].name, username) == 0 && ranks[i].score == score) {
            printf("Your Rank: #%d\n", i + 1);
            break;
        }
    }
}


int time_passed = 0;

int main() {
    signal(SIGINT, pause_game);
    signal(SIGTERM, close_program);

    initscr();
    echo();
    mvprintw(0, 0, "Enter your name: ");
    getnstr(username, sizeof(username));
    noecho();
    clear();

    read_beatmap("beatmaps/testbeatmap.txt");


    if (!audio_init()) {
        fprintf(stderr, "Audio initialization failed\n");
        return 1;
    }

    audio_play_bgm("musics/testbgm.wav"); // TODO: song_path로 바꾸기 (read_beatmap 만든 뒤)
    if (!audio_load_se("sounds/hat.wav")) {
        fprintf(stderr, "SE loading failed\n");
        audio_close();
        return 1;
    }

    init_ncurses();
    srand(time(NULL));

    while (1) {
        clear();

        for (int i = 1; i < NUM_LANES; i++) {
            for (int y = 0; y < HEIGHT; y++) {
                mvaddch(y, i * LANE_WIDTH, '|');
            }
        }
        mvhline(TIMING_LINE, 0, '-', WIDTH);

        draw_notes();
        mvprintw(HEIGHT, 0, "Score: %d", score);
        refresh();

        update_notes();

        int ch = getch();
        if (ch == 'z') break;
        if (ch != ERR) {
            audio_play_se();
            handle_input(ch);
        }

        if (rand() % 10 == 0) {
            spawn_note(rand() % NUM_LANES);
        }

        usleep(160000); 
    }

    endwin();
    handle_game_over(username, score);
    return 0;
}
