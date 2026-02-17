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
    ~GuiRenderer() {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        // Do not call SDL_Quit here; lifecycle managed elsewhere
        if (TTF_WasInit()) {
            TTF_Quit();
        }
    }

    SDL_Renderer* renderer = nullptr;
private:

    TTF_Font* font = nullptr;
};