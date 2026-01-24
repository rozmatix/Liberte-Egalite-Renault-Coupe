#include "MenuState.h"
#include <SDL3/SDL.h>
#include "Gameplay.h"
#include "SaveManager.h"
#include "SoundManager.h"

MenuState::MenuState(Gameplay* game, GuiRenderer* guiRenderer, GuiScene* guiScene)
    : guiRenderer(guiRenderer), guiScene(guiScene), game(game)
{
    // Load and play menu music
    SoundManager::Instance().LoadWav("liberation", "liberation.wav");
    SoundManager::Instance().PlayMusic("liberation", -1);

    titleLabel = new GuiLabel("Liberte Egalite Renault Coupe", { 400, 300 });
    guiScene->labels.push_back(titleLabel);

    SaveData sd;
    saveExists = SaveManager::Load(sd);

    if (saveExists)
        continueLabel = new GuiLabel("Press C to Continue", { 550, 600 });

    startLabel = new GuiLabel("Press ENTER to Start", { 550, 500 });

    guiScene->labels.push_back(startLabel);
    if (continueLabel) guiScene->labels.push_back(continueLabel);
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
        // Stop menu music when leaving menu
        SoundManager::Instance().StopAll();

        // Start gameplay music now (not in Gameplay::Init)
        SoundManager::Instance().PlayMusic("revolution", -1);

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