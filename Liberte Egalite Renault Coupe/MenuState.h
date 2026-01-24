#pragma once
#include "GameState.h"
#include "GuiRenderer.h"
#include "GuiScene.h"
#include "Gameplay.h"
#include "GuiLabel.h"
#include "SaveManager.h"
#include <SDL3/SDL.h>

class MenuState : public GameState
{
public:
    MenuState(Gameplay* game, GuiRenderer* guiRenderer, GuiScene* guiScene);
    void HandleInput(bool keyState[]) override;
    void Update(float dt) override;
    void Draw() override;
    bool IsActive() const override;

private:
    GuiRenderer* guiRenderer;
    GuiScene* guiScene;
    Gameplay* game;

    GuiLabel* titleLabel = nullptr;
    GuiLabel* startLabel = nullptr;
    GuiLabel* continueLabel = nullptr;

    bool active = true;
    bool saveExists = false;

    // Edge detection for keys
    bool prevKeys[SDL_SCANCODE_COUNT] = { false };

    // Start transition flow
    bool startRequested = false;     // set when ENTER or C pressed
    float startDelay = 0.0f;         // countdown delay before switching to gameplay
    static constexpr float kStartDelaySecs = 0.20f; // 200 ms to let button sound play
};