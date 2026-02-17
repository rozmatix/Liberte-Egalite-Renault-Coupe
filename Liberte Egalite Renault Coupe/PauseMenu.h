#pragma once
#include "GuiRenderer.h"
#include "GuiButton.h"
#include "Vector2.h"

class PauseMenu
{
public:
    PauseMenu() = default;

    void Open(GuiRenderer* r);
    void Close();

    bool IsOpen() const { return open; }

    // Handle a mouse click (screen space)
    void HandleClick(float mouseX, float mouseY);

    // Draw the menu
    void Draw();

    // Signals the owner should act on
    bool resumeRequested = false;
    bool saveRequested = false;
    bool quitRequested = false;

private:
    bool open = false;
    GuiRenderer* guiRenderer = nullptr;

    GuiButton* btnResume = nullptr;
    GuiButton* btnSave = nullptr;
    GuiButton* btnQuit = nullptr;
};