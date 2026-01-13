#pragma once
#include <string>
#include "Vector2.h"
#include <SDL3/SDL.h>

class GuiRenderer;

class GuiLabel
{
public:
    GuiLabel(const std::string& text, Vector2 pos);

    void SetText(const std::string& txt);
    void Draw(GuiRenderer& renderer);

private:
    std::string text;
    Vector2 position;
};

