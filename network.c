#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

static int sock;

bool connect_to_server(const char *ip, int port) {
    struct sockaddr_in server_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return false;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    return connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == 0;
}

bool send_score(const char *nickname, const char *title, int score) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "SCORE %s %s %d\n", nickname, title, score);
    return send(sock, buffer, strlen(buffer), 0) > 0;
}

bool request_top10(const char *title, LeaderboardEntry *entries) {
    if (sock < 0) return false;

    char buffer[256];
    snprintf(buffer, sizeof(buffer), "TOP10 %s\n", title);
    send(sock, buffer, strlen(buffer), 0);

    char recv_buf[1024];
    int bytes = recv(sock, recv_buf, sizeof(recv_buf) - 1, 0);
    if (bytes > 0) {
        recv_buf[bytes] = '\0';

        if (strncmp(recv_buf, "No scores found.", 16) == 0) {
            for (int i = 0; i < 10; i++) {
                strcpy(entries[i].nickname, "");
                strcpy(entries[i].title, "");
                entries[i].score = 0;
            }
            return false; // No scores found
        }

        char *line = strtok(recv_buf, "\n");
        int count = 0;
        while (line != NULL && count < 10) {
            sscanf(line, "%s %s %d", entries[count].nickname, entries[count].title, &entries[count].score);
            count++;
            line = strtok(NULL, "\n");
        }
        // Fill remaining entries with empty values if less than 10
        for (; count < 10; count++) {
            strcpy(entries[count].nickname, "");
            strcpy(entries[count].title, "");
            entries[count].score = 0;
        }
        return true;
    }
    return false;
}

void disconnect() {
    close(sock);
}
