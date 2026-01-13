#pragma once
#include "GameState.h"
#include "GuiRenderer.h"
#include "GuiScene.h"
#include "Gameplay.h"
#include "GuiLabel.h"

class MenuState : public GameState
{
public:
    MenuState(GuiRenderer* guiRenderer, GuiScene* guiScene);

    void HandleInput(bool keyState[]) override;
    void Update(float dt) override;
    void Draw() override;
    bool IsActive() const;



private:
    GuiRenderer* guiRenderer;
    GuiScene* guiScene;

    GuiLabel* titleLabel;
    GuiLabel* startLabel;
    bool active = true;
    bool startGame = false;
};