#pragma once
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>

struct LoadedWav {
    SDL_AudioSpec spec{};
    Uint8* buffer = nullptr;   // changed from void* to Uint8*
    Uint32 length = 0;         // keep Uint32 for SDL_LoadWAV
};

struct SoundAsset {
    LoadedWav wav{};
    SDL_AudioStream* stream = nullptr;
    bool loop = false;
    bool bound = false;
};

class SoundManager {
public:
    static SoundManager& Instance();

    bool Init(int freq = 48000, SDL_AudioFormat format = SDL_AUDIO_F32, int channels = 2);
    void Shutdown();

    bool LoadWav(const std::string& name, const std::string& filepath);
    void PlaySound(const std::string& name, float gain = 1.0f);
    void PlayMusic(const std::string& name, float gain = 1.0f);
    void StartLoop(const std::string& name, float gain = 1.0f);
    void StopLoop(const std::string& name);
    void StopAll();
    void Update();

private:
    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec deviceSpec{};
    std::unordered_map<std::string, SoundAsset> assets;
    bool initialized = false;

    void playAsset(SoundAsset& asset, float gain);
};

