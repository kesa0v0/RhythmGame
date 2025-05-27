#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define MAX_ENTRIES 1024

typedef struct {
    char nickname[64];
    char title[64];
    int score;
} Entry;

void sort_scores(const char *filename) {
    Entry entries[MAX_ENTRIES];
    int count = 0;

    FILE *fp = fopen(filename, "r");
    if (!fp) return;

    // 파일에서 읽기
    while (fscanf(fp, "%s %s %d", entries[count].nickname, entries[count].title, &entries[count].score) == 3) {
        count++;
        if (count >= MAX_ENTRIES) break;
    }
    fclose(fp);

    // 점수 기준 내림차순 정렬
    for (int i = 0; i < count - 1; ++i) {
        for (int j = i + 1; j < count; ++j) {
            if (entries[i].score < entries[j].score) {
                Entry tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }

    // 정렬된 데이터 다시 파일에 쓰기
    fp = fopen(filename, "w");
    for (int i = 0; i < count; ++i) {
        fprintf(fp, "%s %s %d\n", entries[i].nickname, entries[i].title, entries[i].score);
    }
    fclose(fp);
}

void* client_handler(void* arg) {
    int client_sock = *(int*)arg;
    free(arg);
    char buffer[1024];

    while (1) {
        int bytes = recv(client_sock, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) break;
        buffer[bytes] = '\0';

        if (strncmp(buffer, "SCORE", 5) == 0) {
            char nickname[64], title[64];
            int score;
            sscanf(buffer + 6, "%s %s %d", nickname, title, &score);
            char filename[128];
            snprintf(filename, sizeof(filename), "%s.txt", title);

            FILE *fp = fopen(filename, "a");
            fprintf(fp, "%s %s %d\n", nickname, title, score);
            fclose(fp);
            sort_scores(filename);
        } else if (strncmp(buffer, "TOP10", 5) == 0) {
            char title[64];
            sscanf(buffer + 6, "%s", title);
            char filename[128];
            snprintf(filename, sizeof(filename), "%s.txt", title);
            FILE *fp = fopen(filename, "r");
            if (fp) {
                char line[MAX_LINE];
                int count = 0;
                char result[1024] = "";
                while (fgets(line, sizeof(line), fp) && count < 10) {
                    strcat(result, line);
                    count++;
                }
                send(client_sock, result, strlen(result), 0);
                fclose(fp);
            } else {
                send(client_sock, "No scores found.\n", 17, 0);
            }
        }
    }
    close(client_sock);
    return NULL;
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9000),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);
    printf("Server is running on port 9000...\n");

    while (1) {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, client_sock);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}