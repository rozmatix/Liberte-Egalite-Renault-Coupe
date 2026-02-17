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

    Vector2 position; // <-- Make position public

    // Add public setter for text
    void SetRawText(const std::string& txt) { text = txt; }
    // Add public getter for text if needed
    const std::string& GetRawText() const { return text; }

private:
    std::string text;
};

