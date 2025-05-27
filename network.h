#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>

bool connect_to_server(const char *ip, int port);
bool send_score(const char *nickname, const char *title, int score);
bool request_top10(const char *title);
void disconnect();

#endif // NETWORK_H
