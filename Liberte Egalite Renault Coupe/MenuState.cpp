#include "MenuState.h"
#include <SDL3/SDL.h>
#include "Gameplay.h"
#include "SaveManager.h"
#include "SoundManager.h"

MenuState::MenuState(Gameplay* game, GuiRenderer* guiRenderer, GuiScene* guiScene)
    : guiRenderer(guiRenderer), guiScene(guiScene), game(game)
{
    // Load and play menu music
    //SoundManager::Instance().LoadWav("liberation", "liberation.wav");
    //SoundManager::Instance().PlayMusic("liberation");

    // Load button sound effect
    SoundManager::Instance().LoadWav("button", "button-heavy.wav");

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
    const bool enterPressed = keyState[SDL_SCANCODE_RETURN] && !prevKeys[SDL_SCANCODE_RETURN];
    const bool cPressed = keyState[SDL_SCANCODE_C] && !prevKeys[SDL_SCANCODE_C];

    if (!startRequested)
    {
        if (enterPressed)
        {
            // Play button sound and request start; delay actual transition
            SoundManager::Instance().PlaySound("button");
            startRequested = true;
            startDelay = kStartDelaySecs;
        }

        if (saveExists && cPressed)
        {
            // Play button sound and request start; load save during delay window
            SoundManager::Instance().PlaySound("button");

            SaveData sd;
            if (SaveManager::Load(sd) && game)
            {
                game->LoadFromSave(sd);
            }
            startRequested = true;
            startDelay = kStartDelaySecs;
        }
    }

    // update prev keys at end
    for (int i = 0; i < SDL_SCANCODE_COUNT; ++i) prevKeys[i] = keyState[i];
}

void MenuState::Update(float dt)
{
    if (startRequested)
    {
        // Count down; let button sound start playing while menu remains active
        startDelay -= dt;
        if (startDelay <= 0.0f)
        {
            // Stop menu audio, start gameplay music, and leave menu
            SoundManager::Instance().StopAll();
            SoundManager::Instance().PlayMusic("revolution");

            guiScene->labels.clear();
            active = false;
        }
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