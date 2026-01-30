#pragma once
#include <vector>
#include "Sprite.h"
#include "Vector2.h"
#include <SDL3/SDL.h>
#include <utility>

class SpriteBatch
{
public:
    SDL_Renderer* renderer;

    // Triples of (sprite, world position, angle)
    struct Entry { Sprite* sprite; Vector2 position; float angle; };
    std::vector<Entry> sprites;

    SpriteBatch(SDL_Renderer* renderer);

    void Add(Sprite* sprite, const Vector2& position, float angle);
    void Draw();
};
