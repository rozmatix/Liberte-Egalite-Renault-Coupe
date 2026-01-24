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
#include "SaveManager.h"

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

    Player* const player() const { return scene.player; }

    // Expose collision callback (call from SimpleColider or Gameplay::Update)
    void OnPlayerEnemyCollision();

    // Fill SaveData from current gameplay state
    void FillSaveData(SaveData& out) const
    {
        out.SoundOn = true; // adapt if you add settings
        if (player())
        {
            out.playerX = player()->position.x;
            out.playerY = player()->position.y;
            out.playerHP = player()->current_hp;
        }
        else
        {
            out.playerX = 0.0f;
            out.playerY = 0.0f;
            out.playerHP = 0;
        }
        out.gameTime = gameTime;

        out.enemies.clear();
        for (const auto& e : scene.enemies)
        {
            EnemySave es;
            es.x = e.position.x;
            es.y = e.position.y;
            es.hp = e.current_hp;
            out.enemies.push_back(es);
        }
    }

    // Apply SaveData to current gameplay state
    void LoadFromSave(const SaveData& in)
    {
        gameTime = in.gameTime;
        if (player())
        {
            player()->position.x = in.playerX;
            player()->position.y = in.playerY;
            player()->current_hp = in.playerHP;
            if (player()->sprite)
                player()->sprite->position = player()->position;
        }

        // restore enemies
        // resize scene.enemies to match saved count, default-construct new enemies if needed
        scene.enemies.resize(in.enemies.size());
        for (size_t i = 0; i < in.enemies.size(); ++i)
        {
            scene.enemies[i].position.x = in.enemies[i].x;
            scene.enemies[i].position.y = in.enemies[i].y;
            scene.enemies[i].current_hp = in.enemies[i].hp;
            // Enemy::Draw / Update should sync sprite position if sprite set via Init
        }
    }

private:
    Scene scene;
    SimpleColider Colider;

    void LoadSpritesFromJSON(const std::string& jsonFile, SDL_Texture* texture);
};