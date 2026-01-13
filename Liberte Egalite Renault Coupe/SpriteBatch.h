#pragma once
#include <vector>
#include "Sprite.h"
#include <SDL3/SDL.h>
#include <cmath>

class SpriteBatch
{
public:
    SDL_Renderer* renderer;
    std::vector<Sprite*> sprites;

    SpriteBatch(SDL_Renderer* renderer);

    void Add(Sprite* sprite);
    void Draw();
};
