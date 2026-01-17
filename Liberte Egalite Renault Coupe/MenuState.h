#pragma once
#include "GameState.h"
#include "GuiRenderer.h"
#include "GuiScene.h"
#include "Gameplay.h"
#include "GuiLabel.h"
#include "SaveManager.h"



class MenuState : public GameState
{
public:
    MenuState(Gameplay* game, GuiRenderer* guiRenderer, GuiScene* guiScene);
    virtual void HandleInput(bool keyState[]) override;
    virtual void Update(float dt) override;
    virtual void Draw() override;
    virtual bool IsActive() const override;

private:
    GuiRenderer* guiRenderer;
    GuiScene* guiScene;
    Gameplay* game;

    GuiLabel* titleLabel = nullptr;
    GuiLabel* startLabel = nullptr;
    GuiLabel* continueLabel = nullptr;

    bool active = true;
    bool startGame = false;

    bool saveExists = false;
};