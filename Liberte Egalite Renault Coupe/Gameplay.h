#pragma once
#include "GameRenderer.h"
#include <vector>
#include <SDL3/SDL.h>
#include "Sprite.h"
#include "Player.h"
#include "Enemy.h"
#include "SimpleColider.h"
#include "Scene.h"
#include "GuiScene.h"
#include "GuiRenderer.h"

class Gameplay
{
public:
    GameRenderer renderer;
    std::vector<Sprite*> sprites;

    bool Init();

    void Update(float dt, bool keyState[]);
    void Draw();

    void ResetGame();
    void CheckRules();

    GuiScene guiScene;
    GuiRenderer guiRenderer;

    GuiLabel* hpLabel = nullptr;
    GuiLabel* timeLabel = nullptr;

    float gameTime = 0.0f;

    Player* player() { return scene.player; }

private:
    Scene scene;
    SimpleColider Colider;

    void LoadSpritesFromJSON(const std::string& jsonFile, SDL_Texture* texture);
};