#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

#define MUSIC_DIR "musics/"
#define MAX_MUSIC_FILES 100

char music_files[MAX_MUSIC_FILES][256];
int music_file_count = 0;
char selected_song_path[512];

void list_music_files() {
    struct dirent *entry;
    DIR *dir = opendir(MUSIC_DIR);

    if (!dir) {
        perror("opendir");
        exit(1);
    }

    struct stat st;
    char fullpath[512];

    while ((entry = readdir(dir)) != NULL) {
        snprintf(fullpath, sizeof(fullpath), "%s%s", MUSIC_DIR, entry->d_name);

        if (stat(fullpath, &st) == 0 && S_ISREG(st.st_mode)) {
            strncpy(music_files[music_file_count], entry->d_name, sizeof(music_files[0]) - 1);
            music_file_count++;
            if (music_file_count >= MAX_MUSIC_FILES) break;
        }
    }

    closedir(dir);
}

const char* select_music() {
    initscr();
    noecho();
    curs_set(0);

    list_music_files();

    for (int i = 0; i < music_file_count; i++) {
        mvprintw(i, 0, "%2d. %s", i + 1, music_files[i]);
    }
    mvprintw(music_file_count + 1, 0, "Select music by number: ");
    refresh();

    int choice;
    echo();
    scanw("%d", &choice);
    noecho();

    endwin();

    if (choice < 1 || choice > music_file_count) {
        fprintf(stderr, "Invalid selection\n");
        exit(1);
    }

    snprintf(selected_song_path, sizeof(selected_song_path), "%s%s", MUSIC_DIR, music_files[choice - 1]);
    return selected_song_path;
}

void generate_beatmap_path(const char *song_path, char *beatmap_path, size_t size) {
    const char *filename = strrchr(song_path, '/');
    if (!filename) filename = song_path;    // 경로 구분자가 없다면 전체가 파일명
    else filename++;                        // '/' 다음이 실제 파일명

    // 파일명 복사
    char base_name[256];
    strncpy(base_name, filename, sizeof(base_name));
    base_name[sizeof(base_name) - 1] = '\0';

    // 확장자 제거 ('.'을 찾아서 잘라냄)
    char *dot = strrchr(base_name, '.');
    if (dot) *dot = '\0';

    // beatmaps 경로 생성
    snprintf(beatmap_path, size, "beatmaps/%s.txt", base_name);
}