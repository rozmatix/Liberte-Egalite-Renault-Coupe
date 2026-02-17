#include "WinLoseMenu.h"
#include <SDL3/SDL.h>
#include <iostream>

void WinLoseMenu::Open(GuiRenderer* r, WinLoseResult result)
{
    if (open) return;
    open = true;
    guiRenderer = r;
    currentResult = result;
    backToMainRequested = false;

    std::cout << "[WinLoseMenu] Open, result="
        << (result == WinLoseResult::Win ? "Win" :
            result == WinLoseResult::Lose ? "Lose" : "None")
        << std::endl;

    float outW = 1500.0f, outH = 1000.0f;
    if (guiRenderer && guiRenderer->renderer)
    {
        int w = 0, h = 0;
        if (SDL_GetRenderOutputSize(guiRenderer->renderer, &w, &h) == 0)
        {
            outW = static_cast<float>(w);
            outH = static_cast<float>(h);
        }
    }

    const char* titleText = (currentResult == WinLoseResult::Win) ? "Zmaga!" : "Poraz!";
    titleLabel = new GuiLabel(titleText, Vector2(outW * 0.5f - 60.0f, outH * 0.5f - 160.0f));

    const Vector2 btnSize{ 320.0f, 64.0f };
    const float cx = outW * 0.5f - btnSize.x * 0.5f;
    const float y = outH * 0.5f - btnSize.y * 0.5f;
    backButton = new GuiButton("Nazaj", Vector2(cx, y), btnSize);
    backButton->SetOnClick([this]()
        {
            std::cout << "[WinLoseMenu] backButton onClick, setting backToMainRequested = true\n";
            backToMainRequested = true;
        });
}

void WinLoseMenu::Close()
{
    std::cout << "[WinLoseMenu] Close\n";

    open = false;

    if (titleLabel)
    {
        delete titleLabel;
        titleLabel = nullptr;
    }

    if (backButton)
    {
        delete backButton;
        backButton = nullptr;
    }

    backToMainRequested = false;
    currentResult = WinLoseResult::None;
    guiRenderer = nullptr;
}

void WinLoseMenu::HandleClick(float mouseX, float mouseY)
{
    if (!open) return;

    std::cout << "[WinLoseMenu] HandleClick at (" << mouseX << ", " << mouseY << ")\n";

    if (backButton)
    {
        backButton->HandleClick(mouseX, mouseY);
        std::cout << "[WinLoseMenu] after HandleClick, backToMainRequested = "
            << (backToMainRequested ? "true" : "false") << "\n";
    }
}

void WinLoseMenu::Draw()
{
    if (!open || !guiRenderer) return;

    // po želji lahko dodaš ozadje (npr. polprozorna barva), za zdaj samo label in gumb
    if (titleLabel)
        titleLabel->Draw(*guiRenderer);
    if (backButton)
        backButton->Draw(*guiRenderer);
}