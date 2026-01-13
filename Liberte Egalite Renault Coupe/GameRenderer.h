#pragma once
#include <SDL3/SDL.h>
#include "SpriteBatch.h"

class GameRenderer
{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;

    SpriteBatch* spriteBatch;

    GameRenderer();
    ~GameRenderer();

    bool Init(const char* title, int width, int height);
    void Clear();
    void Present();
};


