#include "SoundManager.h"
#include <SDL3/SDL.h>
#include <iostream>

SoundManager& SoundManager::Instance() {
    static SoundManager instance;
    return instance;
}

bool SoundManager::Init(int freq, SDL_AudioFormat format, int channels) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        std::cerr << "SDL_INIT_AUDIO failed: " << SDL_GetError() << "\n";
        return false;
    }

    SDL_AudioSpec desired{};
    desired.freq = freq;
    desired.format = format;
    desired.channels = (Uint8)channels;

    device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
    if (device == 0) {
        std::cerr << "SDL_OpenAudioDevice failed: " << SDL_GetError() << "\n";
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    int deviceBufferFrames = 0;
    if (!SDL_GetAudioDeviceFormat(device, &deviceSpec, &deviceBufferFrames)) {
        std::cerr << "SDL_GetAudioDeviceFormat failed: " << SDL_GetError() << "\n";
        SDL_CloseAudioDevice(device);
        device = 0;
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return false;
    }

    SDL_ResumeAudioDevice(device);
    initialized = true;
    return true;
}

void SoundManager::Shutdown() {
    // Guard: if not initialized, nothing to do
    if (!initialized)
        return;

    // Stop loops and unbind streams safely before closing device
    for (auto& kv : assets) {
        SoundAsset& asset = kv.second;
        asset.loop = false;
        if (asset.bound && device != 0) {
            SDL_UnbindAudioStream(asset.stream);
            asset.bound = false;
        }
        if (asset.stream) {
            SDL_ClearAudioStream(asset.stream);
        }
    }

    // Destroy streams and free WAV buffers
    for (auto& kv : assets) {
        SoundAsset& asset = kv.second;
        if (asset.stream) {
            SDL_DestroyAudioStream(asset.stream);
            asset.stream = nullptr;
        }
        if (asset.wav.buffer) {
            SDL_free(asset.wav.buffer);
            asset.wav.buffer = nullptr;
            asset.wav.length = 0;
        }
    }
    assets.clear();

    // Close device last
    if (device != 0) {
        SDL_CloseAudioDevice(device);
        device = 0;
    }

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    initialized = false;
}

bool SoundManager::LoadWav(const std::string& name, const std::string& filepath) {
    if (!initialized) {
        std::cerr << "SoundManager not initialized; cannot load " << filepath << "\n";
        return false;
    }

    SoundAsset asset;
    if (!SDL_LoadWAV(filepath.c_str(), &asset.wav.spec, &asset.wav.buffer, &asset.wav.length)) {
        std::cerr << "SDL_LoadWAV failed (" << filepath << "): " << SDL_GetError() << "\n";
        return false;
    }

    asset.stream = SDL_CreateAudioStream(&asset.wav.spec, &deviceSpec);
    if (!asset.stream) {
        std::cerr << "SDL_CreateAudioStream failed: " << SDL_GetError() << "\n";
        SDL_free(asset.wav.buffer);
        return false;
    }

    assets[name] = asset;
    return true;
}

void SoundManager::playAsset(SoundAsset& asset, float /*gain*/) {
    if (!initialized || device == 0) return;
    if (!asset.stream || !asset.wav.buffer || asset.wav.length == 0) return;

    // If already bound, unbind before reuse
    if (asset.bound) {
        SDL_UnbindAudioStream(asset.stream);
        asset.bound = false;
    }

    SDL_ClearAudioStream(asset.stream);

    if (!SDL_PutAudioStreamData(asset.stream, asset.wav.buffer, asset.wav.length)) {
        std::cerr << "SDL_PutAudioStreamData failed: " << SDL_GetError() << "\n";
        return;
    }

    if (!SDL_FlushAudioStream(asset.stream)) {
        std::cerr << "SDL_FlushAudioStream failed: " << SDL_GetError() << "\n";
        return;
    }

    if (!SDL_BindAudioStream(device, asset.stream)) {
        std::cerr << "SDL_BindAudioStream failed: " << SDL_GetError() << "\n";
        return;
    }

    asset.bound = true;
    asset.loop = false; // one-shot by default
}

void SoundManager::PlaySound(const std::string& name, float /*gain*/) {
    auto it = assets.find(name);
    if (it == assets.end()) {
        std::cerr << "Sound not found: " << name << "\n";
        return;
    }
    playAsset(it->second, 1.0f);
}

void SoundManager::PlayMusic(const std::string& name, float /*gain*/) {
    auto it = assets.find(name);
    if (it == assets.end()) {
        std::cerr << "Music not found: " << name << "\n";
        return;
    }
    playAsset(it->second, 1.0f);
}

void SoundManager::StartLoop(const std::string& name, float /*gain*/) {
    auto it = assets.find(name);
    if (it == assets.end()) {
        std::cerr << "Loop asset not found: " << name << "\n";
        return;
    }
    if (!initialized || device == 0) return;

    SoundAsset& asset = it->second;
    asset.loop = true;

    int available = asset.stream ? SDL_GetAudioStreamAvailable(asset.stream) : 0;
    bool needStart = !asset.bound || available <= 0;
    if (asset.stream && needStart) {
        SDL_ClearAudioStream(asset.stream);
        SDL_PutAudioStreamData(asset.stream, asset.wav.buffer, asset.wav.length);
        SDL_FlushAudioStream(asset.stream);
        if (!asset.bound) {
            SDL_BindAudioStream(device, asset.stream);
            asset.bound = true;
        }
    }
}

void SoundManager::StopLoop(const std::string& name) {
    auto it = assets.find(name);
    if (it == assets.end()) return;

    SoundAsset& asset = it->second;
    asset.loop = false;

    if (asset.bound && device != 0) {
        SDL_UnbindAudioStream(asset.stream);
        asset.bound = false;
    }
    if (asset.stream) {
        SDL_ClearAudioStream(asset.stream);
    }
}

void SoundManager::Update() {
    if (!initialized || device == 0) return;

    // Maintain loops: if a looping asset has no data queued, requeue the buffer.
    for (auto& kv : assets) {
        SoundAsset& asset = kv.second;
        if (!asset.loop || !asset.stream) continue;

        int available = SDL_GetAudioStreamAvailable(asset.stream);
        if (available <= 0) {
            SDL_PutAudioStreamData(asset.stream, asset.wav.buffer, asset.wav.length);
            SDL_FlushAudioStream(asset.stream);

            if (!asset.bound) {
                SDL_BindAudioStream(device, asset.stream);
                asset.bound = true;
            }
        }
    }
}

void SoundManager::StopAll() {
    for (auto& kv : assets) {
        SoundAsset& asset = kv.second;
        asset.loop = false;
        if (asset.bound && device != 0) {
            SDL_UnbindAudioStream(asset.stream);
            asset.bound = false;
        }
        if (asset.stream) {
            SDL_ClearAudioStream(asset.stream);
        }
    }
}
