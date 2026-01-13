#include "GuiRenderer.h"
#include <iostream>

bool GuiRenderer::Init(SDL_Renderer* r)
{
    renderer = r;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Failed: " << SDL_GetError() << "\n";
        return false;
    }

    if (!TTF_Init())
    {
        std::cerr << "TTF_Init Failed: "
            << SDL_GetError() << std::endl;
        return false;
    }
    font = TTF_OpenFont("arial.ttf", 24);
    std::cout << "font" << std::endl;
    if (!font)
    {
        std::cerr << "Failed to load font\n";
        return false;
    }
    return true;
}

void GuiRenderer::DrawText(const std::string& text, Vector2 pos)
{
    SDL_Color color{ 0, 0, 0, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(
        font,
        text.c_str(),
        0,
        color
    );

    if (!surface)
    {
        std::cerr << "TTF_RenderText_Blended failed: "
            << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FRect dst{
        pos.x,
        pos.y,
        (float)surface->w,
        (float)surface->h
    };

    SDL_RenderTexture(renderer, texture, nullptr, &dst);

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}