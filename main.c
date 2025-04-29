#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define HEIGHT 20
#define WIDTH  20

#define NUM_LANES 4
#define NOTE_CHAR 'A'
// #define NOTE_CHAR '■'
#define LANE_WIDTH (WIDTH / NUM_LANES)

typedef struct {
    int lane;   // 0 ~ 3
    int y;      // 현재 y 위치
    int active; // 1이면 존재하는 노트
} Note;

#define MAX_NOTES 100
Note notes[MAX_NOTES];

void spawn_note(int lane) {
    for (int i = 0; i < MAX_NOTES; i++) {
        if (!notes[i].active) {
            notes[i].active = 1;
            notes[i].lane = lane;
            notes[i].y = 0;
            return;
        }
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
        for (int i = 0; i < MAX_NOTES; i++) {
            if (notes[i].active && notes[i].lane == lane) {
                int diff = notes[i].y - (HEIGHT - 3);
                if (diff >= -1 && diff <= 1) { // 판정 범위
                    mvprintw(HEIGHT, 0, "Perfect!                    ");
                    notes[i].active = 0;
                    judged = 1;
                    break;
                }
            }
        }
        if (!judged) {
            mvprintw(HEIGHT, 0, "Miss...                      ");
        }
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
        mvhline(HEIGHT - 3, 0, '-', WIDTH); // 타이밍선

        // 노트 드로우
        draw_notes();

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

        usleep(80000); // 80ms 프레임
    }

    endwin();
    return 0;
}
