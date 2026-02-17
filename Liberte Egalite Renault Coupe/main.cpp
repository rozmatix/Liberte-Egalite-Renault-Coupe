#include "Gameplay.h"
#include "MenuState.h"
#include <SDL3/SDL.h>
#include <vector>
#include <iostream>

enum class OverlayType { MainMenu, Pause, Power };

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Gameplay gameplay;
    if (!gameplay.Init())
        return 1;

    MenuState mainMenu(&gameplay, &gameplay.guiRenderer, &gameplay.guiScene);

    std::vector<OverlayType> stack;
    stack.push_back(OverlayType::MainMenu);

    bool running = true;
    Uint64 prevTicks = SDL_GetTicks();

    bool escWasDownGlobal = false;
    bool inGameplay = false;

    while (running)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if (ev.type == SDL_EVENT_QUIT)
            {
                running = false;
                break;
            }

            if (stack.empty())
                continue;

            const OverlayType top = stack.back();

            if (top == OverlayType::Pause)
            {
                if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT)
                {
                    gameplay.pauseMenu.HandleClick(static_cast<float>(ev.button.x), static_cast<float>(ev.button.y));

                    if (gameplay.pauseMenu.resumeRequested)
                    {
                        gameplay.pauseMenu.Close();
                        if (!stack.empty() && stack.back() == OverlayType::Pause)
                            stack.pop_back();
                        escWasDownGlobal = false;
                    }
                    else if (gameplay.pauseMenu.quitRequested)
                    {
                        running = false;
                    }
                    else if (gameplay.pauseMenu.saveRequested)
                    {
                        SaveData data;
                        gameplay.FillSaveData(data);

                        if (SaveManager::Save(data))
                        {
                            std::cout << "[main] game saved from pause menu\n";
                        }
                        else
                        {
                            std::cout << "[main] ERROR: saving game from pause menu FAILED\n";
                        }

                        gameplay.pauseMenu.saveRequested = false;
                    }
                }
                continue;
            }

            if (top == OverlayType::Power)
            {
                if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT)
                {
                    gameplay.powerMenu.HandleClick(static_cast<float>(ev.button.x), static_cast<float>(ev.button.y));
                }
                continue;
            }

            // MainMenu / gameplay: brez posebne event logike
        }

        if (!running)
            break;

        bool keyState[SDL_SCANCODE_COUNT] = {};
        int num = 0;
        const bool* keys = SDL_GetKeyboardState(&num);
        const int count = (num < SDL_SCANCODE_COUNT) ? num : SDL_SCANCODE_COUNT;
        for (int i = 0; i < count; ++i)
            keyState[i] = keys[i];

        Uint64 now = SDL_GetTicks();
        float dt = (now - prevTicks) / 1000.0f;
        prevTicks = now;

        const bool escDownNow = keys[SDL_SCANCODE_ESCAPE] != 0;
        if (!inGameplay) // v meniju ESC ignoriramo
        {
            escWasDownGlobal = escDownNow;
        }
        else
        {
            // preprosto: ESC v gameplay-ju odpre/zapre pause overlay
            if (escDownNow && !escWasDownGlobal)
            {
                if (stack.empty() || stack.back() != OverlayType::Pause)
                {
                    stack.push_back(OverlayType::Pause);
                    gameplay.pauseMenu.Open(&gameplay.guiRenderer);
                }
                else
                {
                    gameplay.pauseMenu.Close();
                    stack.pop_back();
                }
            }
            escWasDownGlobal = escDownNow;
        }

        if (!stack.empty())
        {
            const OverlayType top = stack.back();

            if (top == OverlayType::MainMenu)
            {
                mainMenu.HandleInput(keyState);
                mainMenu.Update(dt);

                SDL_SetRenderDrawColor(gameplay.renderer.renderer, 255, 255, 255, 255);
                SDL_RenderClear(gameplay.renderer.renderer);
                mainMenu.Draw();
                gameplay.renderer.Present();

                if (!mainMenu.IsActive())
                {
                    stack.pop_back();

                    // ZAČETEK nove igre – pobriši star game-over signal
                    gameplay.gameOverRequested = false;

                    inGameplay = true;
                }
                continue;
            }

            if (top == OverlayType::Pause)
            {
                SDL_SetRenderDrawColor(gameplay.renderer.renderer, 255, 255, 255, 255);
                SDL_RenderClear(gameplay.renderer.renderer);
                gameplay.pauseMenu.Draw();
                gameplay.renderer.Present();
                continue;
            }

            if (top == OverlayType::Power)
            {
                gameplay.powerMenu.Update(dt);

                SDL_SetRenderDrawColor(gameplay.renderer.renderer, 255, 255, 255, 255);
                SDL_RenderClear(gameplay.renderer.renderer);
                gameplay.powerMenu.Draw();
                gameplay.renderer.Present();

                if (!gameplay.powerMenu.IsOpen())
                {
                    if (!stack.empty() && stack.back() == OverlayType::Power)
                        stack.pop_back();
                }
                continue;
            }
        }

        // GAMEPLAY UPDATE/DRAW
        if (inGameplay && !gameplay.gameOverRequested)
        {
            gameplay.Update(dt, keyState);
            gameplay.Draw();
        }

        // GAME OVER → NAZAJ NA GLAVNI MENI
        if (inGameplay && gameplay.gameOverRequested)
        {
            std::cout << "[main] game over, returning to main menu\n";

            inGameplay = false;

            mainMenu.Reset();  // ← meni je spet aktiven

            stack.clear();
            stack.push_back(OverlayType::MainMenu);

            escWasDownGlobal = false;
            continue;
        }
    }

    return 0;
}