#include "Gameplay.h"
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "nlohmann/json.hpp"
#include "SoundManager.h"
using json = nlohmann::json;

bool Gameplay::Init()
{
    if (!renderer.Init("SDL3 Game", 1500, 1000))
        return false;

    // Initialize sound manager
    if (!SoundManager::Instance().Init())
    {
        std::cerr << "SoundManager failed to initialize\n";
        return false;
    }

    // Load sounds and music
    SoundManager::Instance().LoadWav("crash", "car-crash-sound.wav");
    SoundManager::Instance().LoadWav("engine", "engine.wav");
    //SoundManager::Instance().LoadWav("revolution", "revolution.wav");
    //SoundManager::Instance().PlayMusic("revolution", -1);

    SDL_Texture* texture = IMG_LoadTexture(renderer.renderer, "texture.png");
    if (!texture) {
        std::cerr << "Failed to load texture.png" << std::endl;
        return false;
    }

    LoadSpritesFromJSON("texture.json", texture);

    if (!sprites.empty())
    {
        scene.enemies.push_back(Enemy());
        scene.enemies.back().position = Vector2(600, 300);
        scene.enemies.back().Init(sprites[0]);

        scene.player = new Player();
        scene.player->Init(sprites[1]);
        scene.player->position = Vector2(300, 300);
    }
    else
    {
        std::cerr << "ERROR: No sprites loaded, cannot initialize player!" << std::endl;
        return false;
    }

    if (!guiRenderer.Init(renderer.renderer))
    {
        std::cerr << "GuiRenderer failed to load" << std::endl;
        return false;
    }

    // HP – levo zgoraj
    hpLabel = new GuiLabel("HP: 100", Vector2(20, 20));
    guiScene.labels.push_back(hpLabel);

    // TIME – desno zgoraj
    timeLabel = new GuiLabel("Time: 0.0", Vector2(1250, 20));
    guiScene.labels.push_back(timeLabel);

    return true; // brez running
}


void Gameplay::LoadSpritesFromJSON(const std::string& jsonFile, SDL_Texture* texture) {
    std::ifstream iFile(jsonFile);
    if (!iFile.is_open()) {
        std::cerr << "Failed to open " << jsonFile << std::endl;
        return; // brez running
    }

    json j;
    try {
        iFile >> j;
    }
    catch (json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return; // brez running
    }

    float startX = 100.0f;
    float startY = 100.0f;
    float offsetX = 200.0f;

    // Check that textures array exists
    if (!j.contains("textures") || !j["textures"].is_array()) return;

    for (auto& textureEntry : j["textures"]) {
        if (!textureEntry.contains("frames") || !textureEntry["frames"].is_array()) continue;

        for (auto& frame : textureEntry["frames"]) {
            if (!frame.contains("frame")) continue;

            auto f = frame["frame"];

            float w = f.value("w", 0.0f);
            float h = f.value("h", 0.0f);

            if (w <= 0 || h <= 0) continue; // skip empty frames

            SDL_FRect rect;
            rect.x = f.value("x", 0.0f);
            rect.y = f.value("y", 0.0f);
            rect.w = w;
            rect.h = h;

            std::string name = frame.value("filename", "unknown");

            if (!texture) {
                std::cerr << "Texture is null!" << std::endl;
                continue;
            }

            if (rect.w <= 0 || rect.h <= 0) {
                std::cerr << "Skipping frame with w=" << rect.w << ", h=" << rect.h << std::endl;
                continue;
            }

            Sprite* s = new Sprite(name, texture, rect);
            s->position = Vector2(startX, startY);
            s->angle = 0.0f;
            sprites.push_back(s);

            startX += offsetX;
        }
    }
}

void Gameplay::ResetGame()
{
    scene.enemies.clear();
    scene.player->Reset();
    scene.enemies.push_back(Enemy());
    scene.enemies[0].position = Vector2(600, 300);
    scene.enemies[0].Init(sprites[0]);
}

void Gameplay::CheckRules() {

    if (scene.player && scene.player->current_hp <= 0)
    {
        scene.AddAction([this]()
            {
                std::cout << "poraz" << std::endl;
                ResetGame();
            });
        return;
    }

    scene.AddAction([this]()
        {
            scene.enemies.erase(
                std::remove_if(
                    scene.enemies.begin(),
                    scene.enemies.end(),
                    [](const Enemy& e)
                    {
                        return e.current_hp <= 0;
                    }
                ),
                scene.enemies.end()
            );
        });
    if (scene.enemies.empty())
    {
        scene.AddAction([this]()
            {
                std::cout << "zmaga" << std::endl;
                ResetGame();
            });
    }
}

void Gameplay::Update(float dt, bool keyState[])
{
    if (!scene.player) return;

    // player input
    scene.player->HandleInput(keyState, dt);

    // enemies
    for (auto& e : scene.enemies)
    {
        e.AI(scene, dt);
        e.Update(dt);

        if (Colider.DetectCollison(*scene.player, e))
        {
            scene.AddAction([this, &e]()
            {
                const bool tookDamage = scene.player->Damage(20);
                e.Damage(20);
                if (tookDamage)
                {
                    OnPlayerEnemyCollision(); // play crash sound only when player actually took damage
                }
            });
        }
    }

    // Inside Update(float dt, bool keyState[]), after reading input:
    const bool moving =
        keyState[SDL_SCANCODE_W] ||
        keyState[SDL_SCANCODE_A] ||
        keyState[SDL_SCANCODE_S] ||
        keyState[SDL_SCANCODE_D];

    if (moving) {
        SoundManager::Instance().StartLoop("engine");
    } else {
        SoundManager::Instance().StopLoop("engine");
    }

    // Keep loops running
    SoundManager::Instance().Update();

    scene.Update(dt);
    CheckRules();
    scene.ExecuteActions();

    // update GUI
    if (hpLabel)
        hpLabel->SetText("HP: " + std::to_string(scene.player->current_hp));
    if (timeLabel)
        timeLabel->SetText("Time: " + std::to_string((int)gameTime));

    gameTime += dt;
}

void Gameplay::Draw()
{
    renderer.Clear();
    scene.Draw(renderer.spriteBatch);
    guiScene.Draw(guiRenderer);
    renderer.Present();
}

// Add this method implementation to resolve the unresolved external symbol.
void Gameplay::OnPlayerEnemyCollision()
{
    // Play the crash sound on collision
    SoundManager::Instance().PlaySound("crash");
}