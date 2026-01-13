#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "Vector2.h"

class Sprite
{
public:
    std::string name;
    SDL_Texture* texture;

    // Source rectangle within texture
    SDL_FRect source;

    // Position on screen (center of the sprite)
    Vector2 position;

    // Rotation angle in degrees
    float angle;

    Sprite();
    Sprite(const std::string& name, SDL_Texture* tex, const SDL_FRect& src);
};