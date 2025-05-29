#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_ENTRIES 1024
#define MAX_LINE 2048

typedef struct
{
    char nickname[20];
    char title[256];
    int score;
} Entry;

void sort_scores(const char *filename)
{
    Entry entries[MAX_ENTRIES];
    int count = 0;

    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        printf("[sort_scores] 파일 열기 실패: %s\n", filename);
        return;
    }

    // 파일에서 읽기
    while (fscanf(fp, "%s %s %d", entries[count].nickname, entries[count].title, &entries[count].score) == 3)
    {
        count++;
        if (count >= MAX_ENTRIES)
            break;
    }
    fclose(fp);

    printf("[sort_scores] %s에서 %d개의 엔트리 읽음\n", filename, count);
    if (count == 0)
    {
        printf("[sort_scores] 엔트리가 없습니다.\n");
        return;
    }

    // 점수 기준 내림차순 정렬
    for (int i = 0; i < count - 1; ++i)
    {
        for (int j = i + 1; j < count; ++j)
        {
            if (entries[i].score < entries[j].score)
            {
                Entry tmp = entries[i];
                entries[i] = entries[j];
                entries[j] = tmp;
            }
        }
    }

    // 정렬된 데이터 다시 파일에 쓰기
    fp = fopen(filename, "w");
    if (!fp)
    {
        printf("[sort_scores] 파일 쓰기 실패: %s\n", filename);
        return;
    }
    for (int i = 0; i < count; ++i)
    {
        fprintf(fp, "%s %s %d\n", entries[i].nickname, entries[i].title, entries[i].score);
    }
    fclose(fp);

    printf("[sort_scores] %s에 정렬된 데이터 저장 완료\n", filename);
}

void *client_handler(void *arg)
{
    int client_sock = *(int *)arg;
    free(arg);
    char buffer[1024];

    printf("[client_handler] 클라이언트 연결됨 (소켓: %d)\n", client_sock);

    while (1)
    {
        int bytes = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0)
        {
            printf("[client_handler] 클라이언트 연결 종료 (소켓: %d)\n", client_sock);
            break;
        }
        buffer[bytes] = '\0';

        printf("[client_handler] 받은 메시지: %s\n", buffer);

        if (strncmp(buffer, "SCORE", 5) == 0)
        {
            char nickname[64], title[64];
            int score;
            sscanf(buffer + 6, "%s %s %d", nickname, title, &score);
            char filename[128];
            snprintf(filename, sizeof(filename), "%s.txt", title);

            FILE *fp = fopen(filename, "a");
            if (fp)
            {
                fprintf(fp, "%s %s %d\n", nickname, title, score);
                fclose(fp);
                printf("[client_handler/SCORE] 점수 저장: %s %s %d -> %s\n", nickname, title, score, filename);
            }
            else
            {
                printf("[client_handler/SCORE] 점수 파일 열기 실패: %s\n", filename);
            }
            sort_scores(filename);
            send(client_sock, "OK\n", 14, 0);
        }
        else if (strncmp(buffer, "TOP10", 5) == 0)
        {
            char title[64];
            sscanf(buffer + 6, "%s", title);
            char filename[128];
            snprintf(filename, sizeof(filename), "%s.txt", title);
            FILE *fp = fopen(filename, "r");
            if (fp)
            {
                char line[MAX_LINE];
                int count = 0;
                char result[1024] = "";
                while (fgets(line, sizeof(line), fp) && count < 10)
                {
                    strcat(result, line);
                    count++;
                }
                send(client_sock, result, strlen(result), 0);
                fclose(fp);
                printf("[client_handler/TOP10] TOP10 전송: %s (%d개)\n", filename, count);
            }
            else
            {
                send(client_sock, "No scores found.\n", 17, 0);
                printf("[client_handler/TOP10] TOP10 요청 파일 없음: %s\n", filename);
            }
        }
    }
    close(client_sock);
    printf("[client_handler] 클라이언트 소켓 닫힘: %d\n", client_sock);
    return NULL;
}

int main()
{
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(9000),
        .sin_addr.s_addr = INADDR_ANY};

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, 10);
    printf("Server is running on port 9000...\n");

    while (1)
    {
        int *client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, NULL, NULL);
        printf("[main] 클라이언트 접속: 소켓 %d\n", *client_sock);
        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, client_sock);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}