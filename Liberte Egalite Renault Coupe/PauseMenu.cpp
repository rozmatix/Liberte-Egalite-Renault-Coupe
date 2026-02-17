#include "PauseMenu.h"
#include <SDL3/SDL.h>

void PauseMenu::Open(GuiRenderer* r)
{
    if (open) return;
    open = true;
    guiRenderer = r;

    const Vector2 btnSize{ 250.0f, 60.0f };

    // Center based on actual renderer output size (SDL3)
    float outW = 0.0f, outH = 0.0f;
    if (guiRenderer && guiRenderer->renderer)
    {
        int w = 0, h = 0;
        if (SDL_GetRenderOutputSize(guiRenderer->renderer, &w, &h) == 0)
        {
            outW = static_cast<float>(w);
            outH = static_cast<float>(h);
        }
    }
    if (outW <= 0.0f) outW = 1500.0f; // fallback
    if (outH <= 0.0f) outH = 800.0f;  // fallback

    const float cx = outW * 0.5f - btnSize.x * 0.5f;
    const float topY = outH * 0.5f - 90.0f; // roughly center vertically

    btnResume = new GuiButton("Nadaljuj", Vector2(cx, topY + 0.0f),  btnSize);
    btnSave   = new GuiButton("Shrani",   Vector2(cx, topY + 80.0f), btnSize);
    btnQuit   = new GuiButton("Izhod",    Vector2(cx, topY + 160.0f),btnSize);

    btnResume->SetOnClick([this]() { resumeRequested = true; });
    btnSave->SetOnClick([this]()   { saveRequested = true;   });
    btnQuit->SetOnClick([this]()   { quitRequested = true;   });
}

void PauseMenu::Close()
{
    open = false;
    for (GuiButton* b : { btnResume, btnSave, btnQuit })
    {
        if (!b) continue;
        delete b;
    }
    btnResume = btnSave = btnQuit = nullptr;
    resumeRequested = saveRequested = quitRequested = false;
}

void PauseMenu::HandleClick(float mouseX, float mouseY)
{
    if (!open) return;
    if (btnResume) btnResume->HandleClick(mouseX, mouseY);
    if (btnSave)   btnSave->HandleClick(mouseX, mouseY);
    if (btnQuit)   btnQuit->HandleClick(mouseX, mouseY);
}

void PauseMenu::Draw()
{
    if (!open || !guiRenderer) return;
    if (btnResume) btnResume->Draw(*guiRenderer);
    if (btnSave)   btnSave->Draw(*guiRenderer);
    if (btnQuit)   btnQuit->Draw(*guiRenderer);
}