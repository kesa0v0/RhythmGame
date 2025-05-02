#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

bool audio_init();
void audio_play_bgm(const char *path);
void audio_play_se(const char *path);
void audio_close();

bool is_music_playing();
int get_elapsed_ms();
void set_volume(int volume);

#endif