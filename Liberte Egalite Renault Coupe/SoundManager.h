#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>
#include <vector>

struct WavData
{
    SDL_AudioSpec spec{};
    Uint8* buffer = nullptr; // original WAV PCM data
    Uint32 length = 0;       // bytes
};

struct SoundAsset
{
    WavData wav;
    SDL_AudioStream* stream = nullptr; // converts to device format
};

class SoundManager
{
public:
    static SoundManager& Instance();

    bool Init(int freq = 44100, SDL_AudioFormat format = SDL_AUDIO_F32, int channels = 2);
    void Shutdown();

    // Load a WAV file into memory and prepare a stream for playback
    bool LoadWav(const std::string& name, const std::string& filepath);

    // Play a short sound effect (one-shot)
    void PlaySound(const std::string& name, float gain = 1.0f);

    // Play music (non-loop for simplicity; call again to restart)
    void PlayMusic(const std::string& name, float gain = 1.0f);

    // Stop current audio output immediately
    void StopAll();

private:
    SoundManager() = default;
    ~SoundManager() { Shutdown(); }
    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    SDL_AudioDeviceID device = 0;
    SDL_AudioSpec deviceSpec{};
    std::map<std::string, SoundAsset> assets;

    // helper to push full wav buffer into stream and start playback
    void playAsset(SoundAsset& asset, float gain);
};

