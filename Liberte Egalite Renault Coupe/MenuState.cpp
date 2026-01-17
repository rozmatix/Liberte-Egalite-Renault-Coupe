#include "MenuState.h"
#include <SDL3/SDL.h>
#include "Gameplay.h"
#include "SaveManager.h"

MenuState::MenuState(Gameplay* game, GuiRenderer* guiRenderer, GuiScene* guiScene)
    : guiRenderer(guiRenderer), guiScene(guiScene), game(game)
{
    titleLabel = new GuiLabel("Liberte Egalite Renault Coupe", { 400, 300 });
    guiScene->labels.push_back(titleLabel);

    SaveData sd;
    saveExists = SaveManager::Load(sd);

    if (saveExists)
        startLabel = new GuiLabel("Press C to Continue", { 550, 500 });
    else
        startLabel = new GuiLabel("Press ENTER to Start", { 550, 500 });

    guiScene->labels.push_back(startLabel);
}

void MenuState::HandleInput(bool keyState[])
{
    if (keyState[SDL_SCANCODE_RETURN])
        startGame = true;

    if (saveExists && keyState[SDL_SCANCODE_C])
    {
        // load and apply save immediately (game already initialized in main)
        SaveData sd;
        if (SaveManager::Load(sd))
        {
            if (game) game->LoadFromSave(sd);
            startGame = true;
        }
    }
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