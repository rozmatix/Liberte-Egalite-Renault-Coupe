#pragma once
#include <string>
#include <functional>
#include "Vector2.h"
#include <SDL3/SDL.h>

class GuiRenderer;

class GuiButton
{
public:
    GuiButton(const std::string& text, Vector2 pos, Vector2 size)
        : position(pos), size(size), text(text) {}

    void Draw(GuiRenderer& renderer);
    bool HandleClick(float mouseX, float mouseY);

    void SetDisabled(bool value) { disabled = value; }
    bool IsDisabled() const { return disabled; }

    void SetOnClick(std::function<void()> cb) { onClick = std::move(cb); }

    Vector2 position;
    Vector2 size;

private:
    std::string text;
    bool disabled{ false };
    std::function<void()> onClick{};

    inline bool Contains(float x, float y) const
    {
        return x >= position.x && x <= position.x + size.x &&
               y >= position.y && y <= position.y + size.y;
    }
};