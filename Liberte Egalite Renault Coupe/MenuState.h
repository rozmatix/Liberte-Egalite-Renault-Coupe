#pragma once
#include "Vector2.h"

class Gameplay;
class GuiRenderer;
class GuiScene;
class GuiButton;

class MenuState
{
public:
    MenuState(Gameplay* gameplayPtr, GuiRenderer* guiRendererPtr, GuiScene* guiScenePtr);
    ~MenuState();

    void HandleInput(bool keyState[]);
    void Update(float dt);
    void Draw();

    void Activate() { active = true; }
    bool IsActive() const { return active; }

    // Requests for main.cpp
    bool IsNewGameRequested() const { return requestNewGame; }
    void ClearNewGameRequest() { requestNewGame = false; }

    bool IsDisposeRequested() const { return requestDispose; }
    void ClearDisposeRequest() { requestDispose = false; }

    void Reset() { active = true; }

private:
    void StartNewGame();
    void ContinueGame();

    Gameplay* gameplay = nullptr;
    GuiRenderer* guiRenderer = nullptr;
    GuiScene* guiScene = nullptr;

    GuiButton* btnNewGame = nullptr;
    GuiButton* btnContinue = nullptr;

    bool active = true;

    // Added request flags
    bool requestNewGame = false;
    bool requestDispose = false;
};