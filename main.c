#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 20
#define WIDTH  20

#define TIMING_LINE HEIGHT - 4
#define SCORE_LINE HEIGHT
#define HIT_LINE HEIGHT - 1

#define NUM_LANES 4
#define NOTE_CHAR 'A'
// #define NOTE_CHAR '■'
#define LANE_WIDTH (WIDTH / NUM_LANES)

typedef struct {
    int lane;   // 0 ~ 3
    int y;      // 현재 y 위치
    int active; // 1이면 존재하는 노트
} Note;

// Note Queue
#define MAX_NOTES 100
Note notes[MAX_NOTES];
int note_start = 0;
int note_end = 0;
int note_count = 0;


int score = 0;

void spawn_note(int lane) {
    if (note_count < MAX_NOTES) {
        notes[note_end].lane = lane;
        notes[note_end].y = 0; // 시작 위치
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
                notes[i].active = 0; // 화면 아래로 넘어가면 제거
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

void handle_input(int ch) {
    int lane = -1;
    if (ch == 'q') lane = 0;
    else if (ch == 'w') lane = 1;
    else if (ch == 'e') lane = 2;
    else if (ch == 'r') lane = 3;

    if (lane != -1) {
        int judged = 0;
        bool isNoteStartBiggerThanEnd = note_start > note_end;
        int modEnd = isNoteStartBiggerThanEnd? note_end + MAX_NOTES : note_end;
        for (int i = note_start; i < modEnd; i++) {
            if (notes[i % MAX_NOTES].active && notes[i % MAX_NOTES].lane == lane) {
                int diff = notes[i % MAX_NOTES].y - (TIMING_LINE);
                if (diff >= -1 && diff <= 1) { // 판정 범위
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

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);

    srand(time(NULL));
    spawn_note(rand() % NUM_LANES); // 초기 노트 생성
    
    while (1) {
        clear();

        // 배경 그리기
        for (int i = 1; i < NUM_LANES; i++) {
            for (int y = 0; y < HEIGHT; y++) {
                mvaddch(y, i * LANE_WIDTH, '|');
            }
        }
        mvhline(TIMING_LINE, 0, '-', WIDTH); // 타이밍선

        // 노트 드로우
        draw_notes();

        // 점수 업데이트
        mvprintw(HEIGHT, 0, "Score: %d", score);

        refresh();

        update_notes();

        // 입력
        int ch = getch();
        if (ch == 'z') break; // z 누르면 종료
        if (ch != ERR) {
            handle_input(ch);
        }

        // 랜덤 노트 생성
        if (rand() % 10 == 0) { // 약간 랜덤하게 생성
            spawn_note(rand() % NUM_LANES);
        }

        usleep(160000); // 160ms 프레임
        // usleep(80000); // 80ms 프레임
    }

    endwin();
    return 0;
}
