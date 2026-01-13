#include "MenuState.h"
#include <SDL3/SDL.h>

MenuState::MenuState(GuiRenderer* guiRenderer, GuiScene* guiScene)
    : guiRenderer(guiRenderer), guiScene(guiScene)
{
    titleLabel = new GuiLabel("Liberte Egalite Renault Coupe", { 400, 300 });
    guiScene->labels.push_back(titleLabel);

    startLabel = new GuiLabel("Press ENTER to Start", { 550, 500 });
    guiScene->labels.push_back(startLabel);
}

void MenuState::HandleInput(bool keyState[])
{
    if (keyState[SDL_SCANCODE_RETURN])
        startGame = true;
}

void MenuState::Update(float dt)
{
    if (startGame)
    {
        guiScene->labels.clear();
        active = false;
    }
}

void MenuState::Draw()
{
    SDL_SetRenderDrawColor(guiRenderer->renderer, 255, 255, 255, 255);
    SDL_RenderClear(guiRenderer->renderer);

    guiScene->Draw(*guiRenderer);

    SDL_RenderPresent(guiRenderer->renderer);
}

bool MenuState::IsActive() const { return active; }