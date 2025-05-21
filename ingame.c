#include "audio.h"
#include "rank.h"

#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>

#define HEIGHT 20
#define WIDTH  20

#define TIMING_LINE ((HEIGHT) - 4)
#define SCORE_LINE (HEIGHT)
#define HIT_LINE ((HEIGHT) - 1)

#define MS_PER_FRAME 160
#define NUM_LANES 4
#define LANE_WIDTH (WIDTH / NUM_LANES)


int score = 0;

char username[20];
LeaderboardEntry ranks[MAX_RANKS];

bool is_game_paused = false;
int time_passed = 0;


#pragma region Note

#define NOTE_CHAR 'A'

typedef struct Note {
    int lane;
    int y;
    int hit_ms;
    int active;

    struct Note* next;
} Note;
Note* notes = NULL;
Note* notes_back = NULL;
int note_count = 0;

// BeatMapNote 구조체 연결리스트
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
    // 선택된 파일의 데이터를 읽어서 연결리스트로로 구현.
    char buffer[1024];
    ssize_t bytes_read;

    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("파일 열기 실패\n");
        exit(1);
    }

    song_length = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';  // 문자열 종료 처리

        char* line = strtok(buffer, "\n");
        int hit_ms, lane;
        while (line != NULL) {
            if (line[0] == '@') {
                sscanf(line, "@%s %d %s", song_name, &song_length, song_path) == 3;
            }
            else if (line[0] == '#') {
                sscanf(line, "#%d", &bpm) == 1;
            }

            // 비트
            if (sscanf(line, "%d %d", &hit_ms, &lane) == 2) {
                BeatMapNote* new_note = (BeatMapNote*)malloc(sizeof(BeatMapNote));
                new_note->hit_ms = hit_ms;
                new_note->lane = lane;
                new_note->next = NULL;

                if (beatmap == NULL) {
                    beatmap = new_note;
                } else {
                    BeatMapNote* current = beatmap;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = new_note;
                }

                song_length++;
            }

            line = strtok(NULL, "\n");
        }
    }

    close(fd);    

}

void spawn_note(BeatMapNote* beatmap, int lane) {
    Note* new_note = (Note*)malloc(sizeof(Note));
    if (new_note == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        exit(1);
    }
    new_note->lane = lane;
    new_note->hit_ms = beatmap->hit_ms;
    new_note->y = TIMING_LINE - (new_note->hit_ms / bpm);
    new_note->active = 1;
    new_note->next = NULL;

    if (notes == NULL) {
        notes = new_note;
        notes_back = new_note;
    } else {
        notes_back->next = new_note;
        notes_back = new_note;
    }
    note_count++;
}

void update_notes() {
    Note* current = notes;
    while (current != NULL) {
        if (current->active) {
            current->y = TIMING_LINE + (time_passed - current->hit_ms) / bpm;
            if (current->y >= HEIGHT) {
                current->active = 0;
                note_count--;
            }
        }
        current = current->next;
    }

    // release inactive notes
    Note* prev = NULL;
    current = notes;
    while (current != NULL) {
        if (!current->active) {
            if (prev == NULL) {
                notes = current->next;
                free(current);
                current = notes;
            } else {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        } else {
            prev = current;
            current = current->next;
        }
    }
    if (notes == NULL) {
        notes_back = NULL;
    }
}

void draw_notes() {
    Note* current = notes;
    while (current != NULL) {
        if (current->active) {
            int x = current->lane * LANE_WIDTH + LANE_WIDTH / 2;
            mvaddch(current->y, x, NOTE_CHAR);
        }
        current = current->next;
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
        Note* current = notes;
        while (current != NULL) {
            if (current->active && current->lane == lane) {
                int diff = current->y - TIMING_LINE;
                if (diff >= -1 && diff <= 1) {
                    mvprintw(HEIGHT, 0, "Perfect!                    ");
                    current->active = 0;
                    judged = 1;
                    score += 1;
                    break;
                }
            }
            current = current->next;
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

        // Spawn notes based on the beatmap
        BeatMapNote* current = beatmap;
        while (current != NULL) {
            if (time_passed >= current->hit_ms - HEIGHT * MS_PER_FRAME) {
                spawn_note(current, current->lane);
                BeatMapNote* temp = current;
                current = current->next;
                free(temp);
            } else {
                current = current->next;
            }
        }
        beatmap = current;

        // Check for game over condition
        
        // BeatMapNote left is 0 and notes left is 0
        if (beatmap == NULL && notes == NULL) {
            break;
        }

        time_passed += 160; // 60 FPS
        usleep(160000); // 60 FPS 고정
    }

    endwin();
    handle_game_over(username, score);
    return 0;
}
