#include "audio.h"

#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <linux/time.h>
#include <SDL2/SDL_mixer.h>

static int music_started = 0;
static struct timespec music_start_time;

static Mix_Music *bgm = NULL;

bool audio_init()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        fprintf(stderr, "Mix_OpenAudio Error: %s\n", Mix_GetError());
        return false;
    }

    return true;
}
void audio_play_bgm(const char *path)
{
    if (bgm)
        Mix_FreeMusic(bgm);
    bgm = Mix_LoadMUS(path);
    if (!bgm)
    {
        fprintf(stderr, "Mix_LoadMUS Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayMusic(bgm, -1); // 반복 재생
}

void audio_play_se(const char *path)
{
    Mix_Chunk *se = Mix_LoadWAV(path);
    if (!se)
    {
        fprintf(stderr, "Mix_LoadWAV Error: %s\n", Mix_GetError());
        return;
    }
    Mix_PlayChannel(-1, se, 0); // 한번 재생
    Mix_FreeChunk(se);          // 즉시 해제 (메모리 낭비 방지)
}

void audio_close()
{
    if (bgm)
        Mix_FreeMusic(bgm);
    Mix_CloseAudio();
    SDL_Quit();
}

bool is_music_playing()
{
    return Mix_PlayingMusic() != 0;
}

int get_elapsed_ms()
{
    if (music_started)
    {
        struct timespec current_time;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        long elapsed_ns = (current_time.tv_sec - music_start_time.tv_sec) * 1e9 + (current_time.tv_nsec - music_start_time.tv_nsec);
        return elapsed_ns / 1e6; // 밀리초 단위로 변환
    }
    return 0;
}

void set_volume(int volume)
{
    Mix_Volume(-1, volume); // 모든 채널의 볼륨 설정
}