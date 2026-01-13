#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Vector2.h"

class GuiRenderer
{
public:
    bool Init(SDL_Renderer* renderer);
    void DrawText(const std::string& text, Vector2 pos);

    SDL_Renderer* renderer = nullptr;
private:

    TTF_Font* font = nullptr;
};