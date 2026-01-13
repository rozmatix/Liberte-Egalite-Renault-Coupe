#include "Gameplay.h"
#include "MenuState.h"
#include <SDL3/SDL.h>

int main(int argc, char* argv[])
{
    Gameplay game;
    if (!game.Init()) return -1;

    GuiScene menuGuiScene;
    MenuState menu(&game.guiRenderer, &menuGuiScene);

    GameState* currentState = &menu;

    bool running = true;
    SDL_Event e;
    bool keyState[SDL_SCANCODE_COUNT] = { false };

    Uint64 perfFrequency = SDL_GetPerformanceFrequency();
    Uint64 lastCounter = SDL_GetPerformanceCounter();

    while (running)
    {
        Uint64 currentCounter = SDL_GetPerformanceCounter();
        float dt = (float)(currentCounter - lastCounter) / perfFrequency;
        lastCounter = currentCounter;

        // input
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_EVENT_QUIT) running = false;
            if (e.type == SDL_EVENT_KEY_DOWN) keyState[e.key.scancode] = true;
            if (e.type == SDL_EVENT_KEY_UP) keyState[e.key.scancode] = false;
        }

        // update
        if (currentState)
        {
            currentState->HandleInput(keyState);
            currentState->Update(dt);
            if (!currentState->IsActive()) currentState = nullptr;
        }
        else
        {
            game.Update(dt, keyState);
        }

        // draw
        if (currentState)
            currentState->Draw();
        else
            game.Draw();
    }

    return 0;
}