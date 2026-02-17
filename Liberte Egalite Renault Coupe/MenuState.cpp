#include "MenuState.h"
#include "GuiButton.h"
#include "GuiRenderer.h"
#include "GuiScene.h"
#include "SaveManager.h"
#include "Gameplay.h"
#include <SDL3/SDL.h>
#include <filesystem>

namespace
{
    bool HasSave()
    {
        return std::filesystem::exists("save.json");
    }
}

MenuState::MenuState(Gameplay* gameplayPtr, GuiRenderer* guiRendererPtr, GuiScene* guiScenePtr)
    : gameplay(gameplayPtr)
    , guiRenderer(guiRendererPtr)
    , guiScene(guiScenePtr)
{
    const Vector2 btnSize{ 300.0f, 80.0f };
    const Vector2 center{ 1500.0f * 0.5f, 1000.0f * 0.5f };

    btnNewGame = new GuiButton("Nova igra", Vector2(center.x - btnSize.x * 0.5f, center.y - 120.0f), btnSize);
    btnContinue = new GuiButton("Nadaljuj", Vector2(center.x - btnSize.x * 0.5f, center.y + 20.0f), btnSize);

    btnNewGame->SetOnClick([this]() { StartNewGame(); });
    btnContinue->SetOnClick([this]() { ContinueGame(); });

    btnContinue->SetDisabled(!HasSave());
}

MenuState::~MenuState()
{
    delete btnNewGame;
    delete btnContinue;
}

void MenuState::HandleInput(bool keyState[])
{
    (void)keyState;
}

void MenuState::Update(float dt)
{
    (void)dt;

    float mx = 0.0f, my = 0.0f;
    const SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mx, &my);
    if (mouseButtons & SDL_BUTTON_LMASK)
    {
        if (btnNewGame) btnNewGame->HandleClick(mx, my);
        if (btnContinue) btnContinue->HandleClick(mx, my);
    }

    // Keep continue disabled if no save
    if (btnContinue) btnContinue->SetDisabled(!HasSave());
}

void MenuState::Draw()
{
    if (!guiRenderer) return;

    const float screenW = 1500.0f;
    const float titleX = (screenW * 0.5f) - 220.0f;
    const float titleY = 100.0f;
    guiRenderer->DrawText("Liberte Egalite Renault Coupe", Vector2(titleX, titleY));

    if (btnNewGame) btnNewGame->Draw(*guiRenderer);
    if (btnContinue) btnContinue->Draw(*guiRenderer);
}

void MenuState::StartNewGame()
{
    // Reset request just toggles active state, does not affect gameplay or SDL
    active = false;
}

void MenuState::ContinueGame()
{
    if (btnContinue && btnContinue->IsDisabled()) return;
    if (!gameplay) return;

    SaveData loaded{};
    if (SaveManager::Load(loaded))
    {
        gameplay->LoadFromSave(loaded);
        active = false;
    }
}