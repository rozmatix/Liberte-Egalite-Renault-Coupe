#include "SoundManager.h"
#include <SDL3/SDL.h>
#include <iostream>

SoundManager& SoundManager::Instance()
{
    static SoundManager instance;
    return instance;
}

bool SoundManager::Init(int freq, SDL_AudioFormat format, int channels)
{
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO))
    {
        std::cerr << "SDL_INIT_AUDIO failed: " << SDL_GetError() << "\n";
        return false;
    }

    SDL_AudioSpec desired{};
    desired.freq = freq;
    desired.format = format;   // float32 is fine
    desired.channels = (Uint8)channels;

    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
    if (device == 0)
    {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
        return false;
    }

    int deviceBufferFrames = 0;
    if (!SDL_GetAudioDeviceFormat(device, &deviceSpec, &deviceBufferFrames))
    {
        std::cerr << "SDL_GetAudioDeviceFormat failed: " << SDL_GetError() << "\n";
        return false;
    }

    SDL_ResumeAudioDevice(device);
    return true;
}

void SoundManager::Shutdown()
{
    StopAll();

    for (auto& kv : assets)
    {
        if (kv.second.stream)
        {
            SDL_DestroyAudioStream(kv.second.stream);
            kv.second.stream = nullptr;
        }
        if (kv.second.wav.buffer)
        {
            SDL_free(kv.second.wav.buffer);
            kv.second.wav.buffer = nullptr;
        }
    }
    assets.clear();

    if (device)
    {
        SDL_CloseAudioDevice(device);
        device = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

bool SoundManager::LoadWav(const std::string& name, const std::string& filepath)
{
    SoundAsset asset;

    if (!SDL_LoadWAV(filepath.c_str(), &asset.wav.spec, &asset.wav.buffer, &asset.wav.length))
    {
        std::cerr << "SDL_LoadWAV failed (" << filepath << "): " << SDL_GetError() << "\n";
        return false;
    }

    // Create a stream that converts from WAV format to device format
    asset.stream = SDL_CreateAudioStream(&asset.wav.spec, &deviceSpec);
    if (!asset.stream)
    {
        std::cerr << "SDL_CreateAudioStream failed: " << SDL_GetError() << "\n";
        SDL_free(asset.wav.buffer);
        return false;
    }

    assets[name] = asset;
    return true;
}

void SoundManager::playAsset(SoundAsset& asset, float gain)
{
    if (!asset.stream || !asset.wav.buffer || asset.wav.length == 0) return;

    // Reset stream to a known state
    SDL_ClearAudioStream(asset.stream);

    // Optionally adjust gain by scaling samples if source is float32.
    // For simplicity, let device master volume be default 1.0 (no direct gain API).
    // If you need gain, convert data to float and scale before PutAudioStreamData.

    // Push original WAV data into stream
    if (!SDL_PutAudioStreamData(asset.stream, asset.wav.buffer, asset.wav.length))
    {
        std::cerr << "SDL_PutAudioStreamData failed: " << SDL_GetError() << "\n";
        return;
    }

    // Flush so the stream starts producing data
    if (!SDL_FlushAudioStream(asset.stream))
    {
        std::cerr << "SDL_FlushAudioStream failed: " << SDL_GetError() << "\n";
        return;
    }

    // Bind stream to device
    if (!SDL_BindAudioStream(device, asset.stream))
    {
        std::cerr << "SDL_BindAudioStream failed: " << SDL_GetError() << "\n";
        return;
    }
}

void SoundManager::PlaySound(const std::string& name, float /*gain*/)
{
    auto it = assets.find(name);
    if (it == assets.end())
    {
        std::cerr << "Sound not found: " << name << "\n";
        return;
    }
    playAsset(it->second, 1.0f);
}

void SoundManager::PlayMusic(const std::string& name, float /*gain*/)
{
    auto it = assets.find(name);
    if (it == assets.end())
    {
        std::cerr << "Music not found: " << name << "\n";
        return;
    }
    playAsset(it->second, 1.0f);
    // For looping, requeue on completion (poll playback state and restart).
    // SDL3 doesn't auto-loop AudioStream; implement a simple loop if needed.
}

void SoundManager::StopAll()
{
    // Unbind all streams from device (stops playback)
    for (auto& kv : assets)
    {
        if (kv.second.stream)
        {
            SDL_UnbindAudioStream(kv.second.stream);
            SDL_ClearAudioStream(kv.second.stream); // SDL_ClearAudioStream is the correct SDL3 API
        }
    }
}
