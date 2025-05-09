#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

bool audio_init();
void audio_play_bgm(const char *path);
void audio_pause_bgm();
void audio_resume_bgm();
void audio_seek_bgm(int milliseconds);
bool audio_load_se(const char *path);
void audio_play_se();
void audio_close();

bool is_music_playing();
void set_volume(int volume);

#endif