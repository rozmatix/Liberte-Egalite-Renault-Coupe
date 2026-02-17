#pragma once
#include "GameRenderer.h"
#include <vector>
#include <SDL3/SDL.h>
#include <string>
#include "Sprite.h"
#include "Player.h"
#include "Enemy.h"
#include "Scene.h"
#include "GuiScene.h"
#include "GuiRenderer.h"
#include "SaveManager.h"
#include "PhysicsEngine.h"
#include "GuiLabel.h"
#include "GuiButton.h"
#include "PauseMenu.h"
#include "PowerUpMenu.h"
#include "EnemySpawner.h"
#include "WinLoseMenu.h"

struct DamagePopup
{
    GuiLabel* label = nullptr;
    float ttl = 0.0f;
    Vector2 worldPos{};
};

class Gameplay
{
public:
    GameRenderer renderer;
    std::vector<Sprite*> sprites;

    bool Init();
    void Update(float dt, bool keyState[]);
    void Draw();


    void CheckRules();

    GuiScene guiScene;
    GuiRenderer guiRenderer;

    GuiLabel* hpLabel = nullptr;
    GuiLabel* timeLabel = nullptr;
    GuiLabel* levelLabel = nullptr;
    GuiLabel* xpLabel = nullptr;

    // Total session time and countdown target (30 minutes)
    float gameTime = 0.0f;
    const float totalGameLengthSec = 30.0f * 60.0f;

    Player* const player() const { return scene.player; }

    void FillSaveData(SaveData& out) const;
    void LoadFromSave(const SaveData& in);

    // Signal za main: konec igre
    bool gameOverRequested = false;

    // Overlay menus
    PauseMenu pauseMenu;
    PowerUpMenu powerMenu;
    
    bool gameEnded = false;

private:
    Scene scene;
    PhysicsEngine physics;
    void LoadSpritesFromJSON(const std::string& jsonFile, SDL_Texture* texture);

    // Input edge detection
    bool escWasDown = false;
    bool cWasDown = false;

    // Floating damage popups
    std::vector<DamagePopup> damagePopups;

    // Core UI state
    bool paused = false;
    bool powerMenuOpen = false;
    int lastSeenPlayerLevel = 1;

    // In-Gameplay buttons (if needed by menus)
    GuiButton* btnResume = nullptr;
    GuiButton* btnSave   = nullptr;
    GuiButton* btnQuit   = nullptr;

    GuiButton* btnPower1 = nullptr;
    GuiButton* btnPower2 = nullptr;
    GuiButton* btnPower3 = nullptr;
    GuiButton* btnPower4 = nullptr;

    // Player progression stats (referenced in Gameplay.cpp)
    int playerPierceExtra = 0;
    int playerFireLevel = 0;
    int playerBulletSizeLevel = 0;
    int playerMultishotLevel = 0;
    int playerFireRateLevel = 0;
    int playerMaxSpeedLevel = 0;
    int playerExtraHpStacks = 0;
    int playerHpRegenLevel = 0;
    int playerFrostLevel = 0;
    int playerPowerLevel = 0;

    // Timers
    float hpRegenTimer = 0.0f;

    // Spawners
    std::vector<EnemySpawner> spawners;

    // Helpers
    void OpenPowerMenu();
    void ClosePowerMenu();

    void SpawnWaveLikeInit();
    void InitSpawners();
    void UpdateSpawners(float dt);
    void UpdateCountdownHud(float remainingSec);
};