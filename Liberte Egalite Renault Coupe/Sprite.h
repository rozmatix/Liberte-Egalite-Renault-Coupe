#pragma once
#include <SDL3/SDL.h>
#include <string>

class Sprite
{
public:
    std::string name;
    SDL_Texture* texture;

    // Source rectangle within texture
    SDL_FRect source;

    Sprite();
    Sprite(const std::string& name, SDL_Texture* tex, const SDL_FRect& src);
};