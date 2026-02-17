#include "GuiRenderer.h"
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>

bool GuiRenderer::Init(SDL_Renderer* r)
{
    renderer = r;

    if (!TTF_WasInit())
    {
        if (!TTF_Init())
        {
            std::cerr << "TTF_Init Failed: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    if (!font)
    {
        // Prefer an absolute or known relative assets path; fallback to local
        const char* fontPath = "arial.ttf"; // change to "assets/arial.ttf" if that’s where your font lives
        font = TTF_OpenFont(fontPath, 24);
        if (!font)
        {
            std::cerr << "Failed to load font '" << fontPath << "': " << SDL_GetError() << "\n";
            return false;
        }
    }

    return true;
}

void GuiRenderer::DrawText(const std::string& text, Vector2 pos)
{
    if (!font || !renderer) return;
    SDL_Color color{ 0, 0, 0, 255 };

    SDL_Surface* surface = TTF_RenderText_Blended(
        font,
        text.c_str(),
        static_cast<int>(text.length()),
        color
    );
    if (!surface)
    {
        std::cerr << "TTF_RenderText_Blended failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        SDL_DestroySurface(surface);
        return;
    }

    SDL_FRect dst{ pos.x, pos.y, static_cast<float>(surface->w), static_cast<float>(surface->h) };
    SDL_RenderTexture(renderer, texture, nullptr, &dst);
    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);

}