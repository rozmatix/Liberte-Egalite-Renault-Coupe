#pragma once
#include "Vector2.h"
#include "GuiRenderer.h"
#include "GuiButton.h"
#include "GuiLabel.h"

enum class WinLoseResult
{
    None,
    Win,
    Lose
};

class WinLoseMenu
{
public:
    void Open(GuiRenderer* r, WinLoseResult result);
    void Close();
    void HandleClick(float mouseX, float mouseY);
    void Draw();

    bool IsOpen() const { return open; }
    WinLoseResult GetResult() const { return currentResult; }

    // Signal for main: user clicked "back to main menu"
    bool backToMainRequested = false;

private:
    bool open = false;
    GuiRenderer* guiRenderer = nullptr;

    GuiLabel*  titleLabel = nullptr;
    GuiButton* backButton = nullptr;

    WinLoseResult currentResult = WinLoseResult::None;
};