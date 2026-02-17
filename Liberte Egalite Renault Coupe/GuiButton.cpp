#include "GuiButton.h"
#include "GuiRenderer.h"
#include <SDL3/SDL.h>

void GuiButton::Draw(GuiRenderer& rendererRef)
{
    SDL_Renderer* sdl = rendererRef.renderer;
    if (!sdl) return;

    // Light gray colors
    const Uint8 alpha = 255;
    SDL_Color bg = disabled ? SDL_Color{ 160, 160, 160, alpha } : SDL_Color{ 200, 200, 200, alpha };

    // Button background
    SDL_FRect r{ position.x, position.y, size.x, size.y };
    SDL_SetRenderDrawColor(sdl, bg.r, bg.g, bg.b, bg.a);
    SDL_RenderFillRect(sdl, &r);

    // Border
    SDL_SetRenderDrawColor(sdl, 60, 60, 60, 255);
    SDL_RenderRect(sdl, &r);

    // Approximate centered text using small offsets (GuiRenderer::DrawText(text, pos))
    const float textX = position.x + (size.x * 0.5f) - 40.0f; // tweak for horizontal centering
    const float textY = position.y + (size.y * 0.5f) - 16.0f; // tweak for vertical centering
    rendererRef.DrawText(text, Vector2(textX, textY));
}

bool GuiButton::HandleClick(float mouseX, float mouseY)
{
    if (disabled) return false;
    if (Contains(mouseX, mouseY))
    {
        if (onClick) onClick();
        return true;
    }
    return false;
}